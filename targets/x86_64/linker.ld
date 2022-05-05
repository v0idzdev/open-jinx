ENTRY(start)

SECTIONS
{
    . = 1M;

    .boot :
    {
        KEEP(*(.MULTIBOOT_HEADER))
    }

    .text :
    {
        *(.TEXT)
    }
}