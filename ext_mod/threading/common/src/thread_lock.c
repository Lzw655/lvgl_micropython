#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "thread_lock.h"
#include "thread_common.h"


static mp_obj_t threading_lock_acquire(size_t n_args, const mp_obj_t *args)
{
    mp_obj_threading_lock_t *self = MP_OBJ_TO_PTR(args[0]);
    bool wait = true;

    if (n_args > 1) {
        wait = mp_obj_get_int(args[1]);
        // TODO support timeout arg
    }

    int ret = mutex_lock(&self->mutex, wait);

    if (ret == 0) {
        return mp_const_false;
    } else if (ret == 1) {
        self->locked = true;
        return mp_const_true;
    } else {
        mp_raise_OSError(-ret);
    }
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(threading_lock_acquire_obj, 1, 3, threading_lock_acquire);


static mp_obj_t threading_lock__enter__(size_t n_args, const mp_obj_t *args)
{
    return threading_lock_acquire(n_args, args);
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(threading_lock__enter__obj, 1, 1, threading_lock__enter__);


static mp_obj_t threading_lock_release(mp_obj_t self_in)
{
    mp_obj_threading_lock_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->locked) {
        mp_raise_msg(&mp_type_RuntimeError, NULL);
    }
    self->locked = false;

    mutex_unlock(&self->mutex);

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(threading_lock_release_obj, threading_lock_release);


static mp_obj_t threading_lock__exit__(size_t n_args, const mp_obj_t *args)
{
    (void)n_args; // unused
    return threading_lock_release(args[0]);
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(threading_lock__exit__obj, 4, 4, threading_lock__exit__);


static mp_obj_t threading_lock_locked(mp_obj_t self_in)
{
    mp_obj_threading_lock_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(self->locked);
}

MP_DEFINE_CONST_FUN_OBJ_1(threading_lock_locked_obj, threadg_lock_locked);
