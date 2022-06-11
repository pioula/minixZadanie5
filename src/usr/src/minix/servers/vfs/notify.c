#include <fcntl.h>
#include <errno.h>

#include "fs.h"
#include "const.h"
#include "file.h"
#include "vnode.h"

int do_notify(void) {
    printf("WCHODZE %d\n", m_in.m_lc_vfs_notify.fd);
    int event = m_in.m_lc_vfs_notify.event;
    if (event != NOTIFY_OPEN && event != NOTIFY_TRIOPEN &&
        event != NOTIFY_CREATE && event != NOTIFY_MOVE)
        return EINVAL;

    printf("POPRAWNY KOMUNIKAT\n");
    struct filp* filp = get_filp(m_in.m_lc_vfs_notify.fd, VNODE_NONE);
    struct vnode* vnode = filp->filp_vno;
    printf("MAM VNODE\n");

    if (mutex_lock(&vnode->notify_lock) != 0) panic("failed notify");
    printf("MAM LOCKA %d\n", vnode->listeners_count);

    if (vnode->listeners_count >= NR_NOTIFY) {
        if (mutex_unlock(&vnode->notify_lock) != 0) panic("failed notify");
        return ENONOTIFY;
    }

    vnode->listeners_count++;
    printf("MALO LOCKOW %d\n", vnode->listeners_count);
    if (mutex_unlock(&vnode->notify_lock) != 0) panic("failed notify");
    if (cond_wait(&vnode->for_listeners, &vnode->cascade_lock) != 0)
        panic("failed notify");
    
    printf("OBUDZILI MNIE!\n");

    switch (event) {
        case NOTIFY_OPEN: {
            printf("OPEN\n");
            int listeners_count = vnode->listeners_count;
            if (mutex_unlock(&vnode->cascade_lock) != 0)
                panic("unable to unlock mutex");
            printf("OTWARLEM MUTEXA I BUDZE\n");
            return 0;
        }
        case NOTIFY_TRIOPEN: {

        }
        case NOTIFY_CREATE: {

        }
        case NOTIFY_MOVE: {

        }
    }

	return(ENOSYS);  // TODO: implementacja VFS_NOTIFY
}
