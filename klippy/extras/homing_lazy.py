# Home only if not already homed
#
# Copyright 2023 Mikhail Gusarov <dottedmag@dottedmag.net>
#
# This file may be distributed under the terms of the GNU GPLv3 license.

import logging

class PrinterHomingLazy:
    def __init__(self, config):
        self.printer = config.get_printer()

        # Override G28
        self.gcode = self.printer.lookup_object('gcode')
        self.old_G28 = self.gcode.register_command('G28', None)
        self.gcode.register_command('G28', self.cmd_G28)

    def cmd_G28(self, gcmd):
        # Parse arguments to select axes to home
        axes = set()
        for _, axis in enumerate('XYZ'):
            if gcmd.get(axis, None) is not None:
                axes.add(axis)
        if not axes:
            axes = set(['X', 'Y', 'Z'])

        # Skip already-homed axes
        curtime = self.printer.get_reactor().monotonic()
        homed_axes = self.printer.lookup_object('toolhead').get_status(curtime)['homed_axes']
        logging.info("axes to home {}".format(axes))
        logging.info("homed axes {}".format(homed_axes))
        for axis in ['X', 'Y', 'Z']:
            if axis in axes and axis.lower() in homed_axes:
                axes.remove(axis)

        # Nothing to do?
        if len(axes) == 0:
            return

        new_params = {}
        if 'X' in axes:
            new_params['X'] = 0
        if 'Y' in axes:
            new_params['Y'] = 0
        if 'Z' in axes:
            new_params['Z'] = 0
        self.old_G28(self.gcode.create_gcode_command("G28", "G28", new_params))

def load_config(config):
    return PrinterHomingLazy(config)
