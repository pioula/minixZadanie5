#include <fcntl.h>
#include <sys/stat.h>

#include "fs.h"
#include "const.h"
#include "file.h"
#include "vnode.h"
#include "glo.h"
#include "scratchpad.h"

/*===========================================================================*
 *				wake_listeners				     *
 *===========================================================================*/
void wake_listeners(vp, why)
struct vnode *vp;
int why;
{
  if (listeners > 0) {
    release(vp, why, listeners);
  }
}

/*===========================================================================*
 *				do_notify				     *
 *===========================================================================*/
int do_notify(void) {
    int event = m_in.m_lc_vfs_notify.event;
    if (event != NOTIFY_OPEN && event != NOTIFY_TRIOPEN &&
        event != NOTIFY_CREATE && event != NOTIFY_MOVE)
        return EINVAL;

    struct filp* filp = get_filp(m_in.m_lc_vfs_notify.fd, VNODE_NONE);
    if (err_code != OK)
        return err_code;

    if (listeners + 1 > NR_NOTIFY) {
        return ENONOTIFY;
    }

    if (event == NOTIFY_OPEN) {
        scratch(fp).file.fd_nr = m_in.m_lc_vfs_notify.fd;
        suspend(FP_BLOCKED_ON_NOTIFY_OPEN);
        return (SUSPEND);
    }
    if (event == NOTIFY_TRIOPEN) {
        if (filp->filp_vno->v_ref_count >= 3)
            return (OK);
        scratch(fp).file.fd_nr = m_in.m_lc_vfs_notify.fd;
        suspend(FP_BLOCKED_ON_NOTIFY_TRIOPEN);
        return (SUSPEND);
    }

    if (!S_ISDIR(filp->filp_vno->v_mode))
        return ENOTDIR;

    if (event == NOTIFY_CREATE) {
        scratch(fp).file.filp = filp;
        suspend(FP_BLOCKED_ON_NOTIFY_CREATE);
        return (SUSPEND);
    }
    
    if (event == NOTIFY_MOVE) {
        scratch(fp).file.filp = filp;
        suspend(FP_BLOCKED_ON_NOTIFY_MOVE);
        return (SUSPEND);
    }

    return EINVAL;
}
