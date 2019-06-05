#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
from bincrafters import build_template_default, build_template_header_only


if __name__ == "__main__":
    header_only = not os.getenv("CONAN_OPTIONS", False)

    builder = build_template_default.get_builder(pure_c=False)

    if header_only:
        builder = build_template_header_only.get_builder()

    builder.run()

