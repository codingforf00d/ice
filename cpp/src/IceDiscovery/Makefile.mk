#
# Copyright (c) ZeroC, Inc. All rights reserved.
#

$(project)_libraries := IceDiscovery

IceDiscovery_targetdir                  := $(libdir)
IceDiscovery_dependencies               := Ice
IceDiscovery_cppflags                   := -DICE_DISCOVERY_API_EXPORTS

projects += $(project)
