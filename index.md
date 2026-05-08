---
title: Home
layout: home
nav_order: 1
# The site root. We deliberately do NOT duplicate the README's content
# into this file. Instead we pull it in at build time with
# `include_relative`, so this page always matches whatever is on the
# repo front page without manual sync. The trade-off is that Liquid
# tags inside the README (`{{ ... }}` / `{% ... %}`) would be
# interpreted — but the README contains neither today and we'd catch
# it instantly if one were added (Jekyll build fails loudly).
#
# Note: relative image paths inside the included README (e.g.
# `doc/exec_pipeline_example.png`) resolve against THIS file's
# location (the repo root), not against the README's location —
# which happens to be the same directory, so everything lines up.
---

{% include_relative README.md %}
