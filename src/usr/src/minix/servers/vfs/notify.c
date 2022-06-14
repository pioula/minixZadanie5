#include <fcntl.h>
#include <errno.h>

#include "fs.h"
#include "const.h"
#include "file.h"
#include "vnode.h"
#include "glo.h"
#include "scratchpad.h"

int do_notify(void) {
    int event = m_in.m_lc_vfs_notify.event;
    if (event != NOTIFY_OPEN && event != NOTIFY_TRIOPEN &&
        event != NOTIFY_CREATE && event != NOTIFY_MOVE)
        return -EINVAL;

    struct filp* filp = get_filp(m_in.m_lc_vfs_notify.fd, VNODE_NONE);
    
    if (listeners + 1 > NR_NOTIFY) {
        return -ENONOTIFY;
    }

    scratch(fp).file.filp = filp;
    suspend(FP_BLOCKED_ON_NOTIFY_OPEN);
    return (SUSPEND);
}
