#include <stdio.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv, char **envp) {
    // What we really need is selinuxfs on /sys/fs/selinux, because bionic will require /sys/fs/selinux/null for pid != 1
    mount("sysfs", "/sys", "sysfs", 0, NULL);
    mount("selinuxfs", "/sys/fs/selinux", "selinuxfs", 0, NULL);

    // But to provide the same comfort on SaR non-dynamic as SaR dynamic, mount the expected virtual FS
    // Reference: system/core/init/first_stage_init.cpp FirstStageMain()
    mount("tmpfs", "/dev", "tmpfs", MS_NOSUID, "mode=0755");
    mkdir("/dev/pts", 0755);
    mkdir("/dev/socket", 0755);
    mount("devpts", "/dev/pts", "devpts", 0, NULL);
    mount("proc", "/proc", "proc", 0, "hidepid=2,gid=3009");
    chmod("/proc/cmdline", 0440);
    gid_t groups[] = {3009};
    setgroups(1, groups);
    mknod("/dev/kmsg", S_IFCHR | 0600, makedev(1, 11));
    mknod("/dev/random", S_IFCHR | 0666, makedev(1, 8));
    mknod("/dev/urandom", S_IFCHR | 0666, makedev(1, 9));
    mknod("/dev/ptmx", S_IFCHR | 0666, makedev(5, 2));
    mknod("/dev/null", S_IFCHR | 0666, makedev(1, 3));
    mount("tmpfs", "/mnt", "tmpfs", MS_NOEXEC | MS_NOSUID | MS_NODEV,
                    "mode=0755,uid=0,gid=1000");
    mkdir("/mnt/vendor", 0755);
    mkdir("/mnt/product", 0755);
    mount("tmpfs", "/debug_ramdisk", "tmpfs", MS_NOEXEC | MS_NOSUID | MS_NODEV,
                    "mode=0755,uid=0,gid=0");


    // TODO: first_stage_init's DoFirstMount
    // Read fstab from /sys/firmware/fdt to mount partitions

    // TODO: "vendor" partition remains to be mounted
    // TODO: "/dev" remains to be filled (this is a /dev that will be used by Android second stage init)
    // Those things will probably best better done from init script itself rather than from this C++ file

    char *new_argv[] = { (char*)"init", NULL };
    execve("/system/bin/init", new_argv, envp);
}
