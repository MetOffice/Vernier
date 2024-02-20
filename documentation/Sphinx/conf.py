# Configuration file for the Sphinx documentation builder.
#
# For a full
# list of options see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))

# -- Project information -----------------------------------------------------

project = 'vernier'
copyright = 'Crown Copyright 2023 Met Office. All rights reserved.'
author = ''

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here. 


# Add any paths that contain templates here.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
exclude_patterns = []

# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.
html_theme = 'sphinx_rtd_theme'
html_theme_options = {
    'canonical_url': '',
    'display_version': True,
    'style_external_links': False,
    'logo_only': False,

    # Toc (table of contents) options
    'collapse_navigation': False,
    'sticky_navigation': True,
    'navigation_depth': 3,
    'includehidden': True,
    'titles_only': False
}
github_url = 'https://github.com/MetOffice/Vernier'

# Add any paths that contain custom static files here,
# relative to this directory.
html_static_path = []

html_permalinks = False

