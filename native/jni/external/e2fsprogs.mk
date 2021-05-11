LOCAL_PATH := $(call my-dir)

# Libraries and executables from Android's e2fsprogs fork
include $(CLEAR_VARS)
LOCAL_MODULE := libext2_uuid

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/e2fsprogs/lib \
    $(LOCAL_PATH)/e2fsprogs/lib/uuid \
    
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := \
    e2fsprogs/lib/uuid/clear.c \
    e2fsprogs/lib/uuid/compare.c \
    e2fsprogs/lib/uuid/copy.c \
    e2fsprogs/lib/uuid/gen_uuid.c \
    e2fsprogs/lib/uuid/isnull.c \
    e2fsprogs/lib/uuid/pack.c \
    e2fsprogs/lib/uuid/parse.c \
    e2fsprogs/lib/uuid/unpack.c \
    e2fsprogs/lib/uuid/unparse.c \
    e2fsprogs/lib/uuid/uuid_time.c \
    
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libext2_com_err

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/e2fsprogs/lib \
    $(LOCAL_PATH)/e2fsprogs/lib/uuid \

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := \
    e2fsprogs/lib/et/error_message.c \
    e2fsprogs/lib/et/et_name.c \
    e2fsprogs/lib/et/init_et.c \
    e2fsprogs/lib/et/com_err.c \
    e2fsprogs/lib/et/com_right.c \
    
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libext2_e2p
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/e2fsprogs/lib \
    $(LOCAL_PATH)/e2fsprogs/lib/e2p \
    
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := \
    e2fsprogs/lib/e2p/encoding.c \
    e2fsprogs/lib/e2p/feature.c \
    e2fsprogs/lib/e2p/fgetflags.c \
    e2fsprogs/lib/e2p/fsetflags.c \
    e2fsprogs/lib/e2p/fgetproject.c \
    e2fsprogs/lib/e2p/fsetproject.c \
    e2fsprogs/lib/e2p/fgetversion.c \
    e2fsprogs/lib/e2p/fsetversion.c \
    e2fsprogs/lib/e2p/getflags.c \
    e2fsprogs/lib/e2p/getversion.c \
    e2fsprogs/lib/e2p/hashstr.c \
    e2fsprogs/lib/e2p/iod.c \
    e2fsprogs/lib/e2p/ljs.c \
    e2fsprogs/lib/e2p/ls.c \
    e2fsprogs/lib/e2p/mntopts.c \
    e2fsprogs/lib/e2p/parse_num.c \
    e2fsprogs/lib/e2p/pe.c \
    e2fsprogs/lib/e2p/pf.c \
    e2fsprogs/lib/e2p/ps.c \
    e2fsprogs/lib/e2p/setflags.c \
    e2fsprogs/lib/e2p/setversion.c \
    e2fsprogs/lib/e2p/uuid.c \
    e2fsprogs/lib/e2p/ostype.c \
    e2fsprogs/lib/e2p/percent.c \
        
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libext2_blkid
LOCAL_STATIC_LIBRARIES := libext2_uuid

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/e2fsprogs/lib \
    $(LOCAL_PATH)/e2fsprogs/lib/blkid \
    
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := \
    e2fsprogs/lib/blkid/cache.c \
    e2fsprogs/lib/blkid/dev.c \
    e2fsprogs/lib/blkid/devname.c \
    e2fsprogs/lib/blkid/devno.c \
    e2fsprogs/lib/blkid/getsize.c \
    e2fsprogs/lib/blkid/llseek.c \
    e2fsprogs/lib/blkid/probe.c \
    e2fsprogs/lib/blkid/read.c \
    e2fsprogs/lib/blkid/resolve.c \
    e2fsprogs/lib/blkid/save.c \
    e2fsprogs/lib/blkid/tag.c \
    e2fsprogs/lib/blkid/version.c \
        
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libext2fs
LOCAL_STATIC_LIBRARIES := \
    libext2_com_err \
    libext2_uuid \
    libsparse \

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/e2fsprogs/lib \
    $(LOCAL_PATH)/e2fsprogs/lib/ext2fs \
    
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := \
    e2fsprogs/lib/ext2fs/ext2_err.c \
    e2fsprogs/lib/ext2fs/alloc.c \
    e2fsprogs/lib/ext2fs/alloc_sb.c \
    e2fsprogs/lib/ext2fs/alloc_stats.c \
    e2fsprogs/lib/ext2fs/alloc_tables.c \
    e2fsprogs/lib/ext2fs/atexit.c \
    e2fsprogs/lib/ext2fs/badblocks.c \
    e2fsprogs/lib/ext2fs/bb_inode.c \
    e2fsprogs/lib/ext2fs/bitmaps.c \
    e2fsprogs/lib/ext2fs/bitops.c \
    e2fsprogs/lib/ext2fs/blkmap64_ba.c \
    e2fsprogs/lib/ext2fs/blkmap64_rb.c \
    e2fsprogs/lib/ext2fs/blknum.c \
    e2fsprogs/lib/ext2fs/block.c \
    e2fsprogs/lib/ext2fs/bmap.c \
    e2fsprogs/lib/ext2fs/check_desc.c \
    e2fsprogs/lib/ext2fs/crc16.c \
    e2fsprogs/lib/ext2fs/crc32c.c \
    e2fsprogs/lib/ext2fs/csum.c \
    e2fsprogs/lib/ext2fs/closefs.c \
    e2fsprogs/lib/ext2fs/dblist.c \
    e2fsprogs/lib/ext2fs/dblist_dir.c \
    e2fsprogs/lib/ext2fs/digest_encode.c \
    e2fsprogs/lib/ext2fs/dirblock.c \
    e2fsprogs/lib/ext2fs/dirhash.c \
    e2fsprogs/lib/ext2fs/dir_iterate.c \
    e2fsprogs/lib/ext2fs/dupfs.c \
    e2fsprogs/lib/ext2fs/expanddir.c \
    e2fsprogs/lib/ext2fs/ext_attr.c \
    e2fsprogs/lib/ext2fs/extent.c \
    e2fsprogs/lib/ext2fs/fallocate.c \
    e2fsprogs/lib/ext2fs/fileio.c \
    e2fsprogs/lib/ext2fs/finddev.c \
    e2fsprogs/lib/ext2fs/flushb.c \
    e2fsprogs/lib/ext2fs/freefs.c \
    e2fsprogs/lib/ext2fs/gen_bitmap.c \
    e2fsprogs/lib/ext2fs/gen_bitmap64.c \
    e2fsprogs/lib/ext2fs/get_num_dirs.c \
    e2fsprogs/lib/ext2fs/get_pathname.c \
    e2fsprogs/lib/ext2fs/getsize.c \
    e2fsprogs/lib/ext2fs/getsectsize.c \
    e2fsprogs/lib/ext2fs/hashmap.c \
    e2fsprogs/lib/ext2fs/i_block.c \
    e2fsprogs/lib/ext2fs/icount.c \
    e2fsprogs/lib/ext2fs/imager.c \
    e2fsprogs/lib/ext2fs/ind_block.c \
    e2fsprogs/lib/ext2fs/initialize.c \
    e2fsprogs/lib/ext2fs/inline.c \
    e2fsprogs/lib/ext2fs/inline_data.c \
    e2fsprogs/lib/ext2fs/inode.c \
    e2fsprogs/lib/ext2fs/io_manager.c \
    e2fsprogs/lib/ext2fs/ismounted.c \
    e2fsprogs/lib/ext2fs/link.c \
    e2fsprogs/lib/ext2fs/llseek.c \
    e2fsprogs/lib/ext2fs/lookup.c \
    e2fsprogs/lib/ext2fs/mmp.c \
    e2fsprogs/lib/ext2fs/mkdir.c \
    e2fsprogs/lib/ext2fs/mkjournal.c \
    e2fsprogs/lib/ext2fs/namei.c \
    e2fsprogs/lib/ext2fs/native.c \
    e2fsprogs/lib/ext2fs/newdir.c \
    e2fsprogs/lib/ext2fs/nls_utf8.c \
    e2fsprogs/lib/ext2fs/openfs.c \
    e2fsprogs/lib/ext2fs/progress.c \
    e2fsprogs/lib/ext2fs/punch.c \
    e2fsprogs/lib/ext2fs/qcow2.c \
    e2fsprogs/lib/ext2fs/rbtree.c \
    e2fsprogs/lib/ext2fs/read_bb.c \
    e2fsprogs/lib/ext2fs/read_bb_file.c \
    e2fsprogs/lib/ext2fs/res_gdt.c \
    e2fsprogs/lib/ext2fs/rw_bitmaps.c \
    e2fsprogs/lib/ext2fs/sha256.c \
    e2fsprogs/lib/ext2fs/sha512.c \
    e2fsprogs/lib/ext2fs/swapfs.c \
    e2fsprogs/lib/ext2fs/symlink.c \
    e2fsprogs/lib/ext2fs/undo_io.c \
    e2fsprogs/lib/ext2fs/unix_io.c \
    e2fsprogs/lib/ext2fs/sparse_io.c \
    e2fsprogs/lib/ext2fs/unlink.c \
    e2fsprogs/lib/ext2fs/valid_blk.c \
    e2fsprogs/lib/ext2fs/version.c \
    e2fsprogs/lib/ext2fs/test_io.c \
        
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libext2_quota
LOCAL_STATIC_LIBRARIES := \
    libext2fs \
    libext2_blkid \
    libext2_com_err \
    
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/e2fsprogs/lib \
    $(LOCAL_PATH)/e2fsprogs/lib/support \
    
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := \
    e2fsprogs/lib/support/dict.c \
    e2fsprogs/lib/support/mkquota.c \
    e2fsprogs/lib/support/parse_qtype.c \
    e2fsprogs/lib/support/plausible.c \
    e2fsprogs/lib/support/profile.c \
    e2fsprogs/lib/support/profile_helpers.c \
    e2fsprogs/lib/support/prof_err.c \
    e2fsprogs/lib/support/quotaio.c \
    e2fsprogs/lib/support/quotaio_tree.c \
    e2fsprogs/lib/support/quotaio_v2.c \
        
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libext2_misc
LOCAL_STATIC_LIBRARIES := \
    libext2fs \
    libext2_com_err \
    libext2_quota \
    
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/e2fsprogs/misc \
    
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := \
    e2fsprogs/misc/create_inode.c \
    
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := mke2fs
LOCAL_STATIC_LIBRARIES := \
    libext2fs \
    libext2_blkid \
    libext2_misc \
    libext2_uuid \
    libext2_quota \
    libext2_com_err \
    libext2_e2p \
    
LOCAL_SRC_FILES := \
    e2fsprogs/misc/mke2fs.c \
    e2fsprogs/misc/util.c \
    e2fsprogs/misc/mk_hugefiles.c \
    e2fsprogs/misc/default_profile.c \
    
LOCAL_LDLIBS := -ldl -lz
    
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := e2fsdroid
LOCAL_STATIC_LIBRARIES := \
    libext2_com_err \
    libext2_misc \
    libext2fs \
    libbase \
    libselinux \
    libcutils \
    
LOCAL_SRC_FILES := \
    e2fsprogs/contrib/android/e2fsdroid.c \
    e2fsprogs/contrib/android/block_range.c \
    e2fsprogs/contrib/android/fsmap.c \
    e2fsprogs/contrib/android/block_list.c \
    e2fsprogs/contrib/android/base_fs.c \
    e2fsprogs/contrib/android/perms.c \
    e2fsprogs/contrib/android/basefs_allocator.c \
    
# We have to undefine the __ANDROID__ macro here to avoid
# calling into android-specific libselinux functions, which
# require the full Android filesystem hierarchy (i.e. the
# sepolicy files)
# This does not affect the functionalities we want to use
LOCAL_CFLAGS := -D_GNU_SOURCE -U__ANDROID__
    
LOCAL_LDLIBS := -ldl -lz
    
include $(BUILD_EXECUTABLE)
