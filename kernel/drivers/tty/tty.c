#include <drivers/tty/tty.h>

TTY_t **ttys;
uint8_t currentTTYid;
TTY_t *currentTTY;

void init_tty()
{
    if (ttys == NULL)
    {
        ttys = kmalloc(MAX_TTYS * sizeof(TTY_t *));
        if (ttys == NULL)
        {
            debug_log(__FILE__, __LINE__, __func__, "Failed to allocate memory for TTY array!");
            return;
        }

        for (size_t i = 0; i < MAX_TTYS; i++)
        {
            ttys[i] = NULL;
        }
    }
    else
    {
        for (size_t i = 0; i < MAX_TTYS; i++)
        {
            if (ttys[i] != NULL)
            {
                kfree(ttys[i]);
                ttys[i] = NULL;
            }
        }
    }

    cdebug_log(__func__, "done.");
}

void tty_spawn(uint8_t id)
{
    if (id >= MAX_TTYS || ttys[id] != NULL)
    {
        return;
    }

    ttys[id] = kmalloc(sizeof(TTY_t));
    if (ttys[id] == NULL)
    {
        debug_log(__FILE__, __LINE__, __func__, "Failed to allocate memory for tty%04d!", id);
        return;
    }

    currentTTYid = 0;
    currentTTY = ttys[currentTTYid];
    struct nighterm_ctx *context = kmalloc(sizeof(struct nighterm_ctx));
    currentTTY->ctx = context;
    int s = nighterm_initialize(currentTTY->ctx,
                                NULL,
                                framebuffer->address,
                                framebuffer->width,
                                framebuffer->height,
                                framebuffer->pitch,
                                framebuffer->bpp,
                                kmalloc, kfree);

    if (s != NIGHTERM_SUCCESS)
    {
        debug_log(__FILE__, __LINE__, __func__, "Failed to initialize nighterm for tty%04d!", id);
        return;
    }

    tty_switch(id);
    cdebug_log(__func__, "tty%04d", id);
}

void tty_switch(uint8_t id)
{
    ttys[id]->id = id;
    currentTTYid = id;
    currentTTY = ttys[id];
    tty_flush();
}

void tty_flush()
{
    if (currentTTY != NULL && currentTTY->ctx != NULL)
    {
        nighterm_flush(currentTTY->ctx, 0, 0, 0);
        nighterm_set_cursor_position(currentTTY->ctx, 0, 0);
    }
}

void tty_write(char ch)
{
    if (currentTTY != NULL && currentTTY->ctx != NULL)
    {
        nighterm_write(currentTTY->ctx, ch);
    }
}