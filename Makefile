# ----------------------------
# Makefile Options
# ----------------------------

NAME = MINES
ICON = iconc.png
DESCRIPTION = "Minesweeper"
COMPRESSED = YES
ARCHIVED = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)