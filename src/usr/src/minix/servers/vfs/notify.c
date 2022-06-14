#include <fcntl.h>
#include <errno.h>

#include "fs.h"
#include "const.h"
#include "file.h"
#include "vnode.h"
#include "glo.h"
#include "scratchpad.h"

int do_notify(void) {
    printf("WCHODZE %d %d\n", m_in.m_lc_vfs_notify.fd, job_m_in.m_type);
    int event = m_in.m_lc_vfs_notify.event;
    if (event != NOTIFY_OPEN && event != NOTIFY_TRIOPEN &&
        event != NOTIFY_CREATE && event != NOTIFY_MOVE)
        return -EINVAL;

    printf("POPRAWNY KOMUNIKAT\n");
    struct filp* filp = get_filp(m_in.m_lc_vfs_notify.fd, VNODE_NONE);
    
    struct vnode* vnode = filp->filp_vno;
    printf("MAM VNODE\n");
    printf("MAM LOCKA %d\n", vnode->listeners_count);
    
    if (listeners + 1 > NR_NOTIFY) {
        puts("JAHA");
        return -ENONOTIFY;
    }

    listeners++;

    vnode->listeners_count++;
    printf("MALO LOCKOW %d\n", vnode->listeners_count);

    scratch(fp).file.filp = filp;
    suspend(FP_BLOCKED_ON_NOTIFY_OPEN);
    return (SUSPEND);
}
