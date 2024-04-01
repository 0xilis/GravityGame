# Gravity
Gravity Game (SDL2)

# Compatibility

MacOS, Linux

# Contributing

I'll get this finished later...

### limits.h

Ok, so this is a weird rule for contributing, I know, but: please never use limits.h. Use defs.h, as it already has all limits pre-defined for you. This is because on my Arch Linux machine, limits.h looks for <linux/limits.h> which is not present, meaning I can't compile anything with limits.h, hence why I just pre-define all limits in defs.h. I know I should probably fix my build system rather than just requiring contributors to apply to my needs, but I cannot for the life of me figure out how to fix this, sorry :P.