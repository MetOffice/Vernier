# Configuration file for the Sphinx documentation builder.
#
# For a full
# list of options see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

import os

# -- Project information -----------------------------------------------------

project = 'Vernier'
copyright = '2023 Met Office. All rights reserved'
author = ''

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here.
extensions = ["sphinx_sitemap"]

# Add any paths that contain templates here.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
exclude_patterns = []

language = "en"

# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.
html_theme = 'pydata_sphinx_theme'
html_title = "Vernier"

# Generate the sitemap info, this will need updating when we have versioned docs
html_baseurl = os.environ.get("SPHINX_HTML_BASE_URL", "https://metoffice.github.io/Vernier")
sitemap_locales = [None]
sitemap_url_scheme = "{link}"

# Hide the link which shows the rst markup
html_show_sourcelink = False

html_theme_options = {
    "navigation_with_keys": True,
    "use_edit_page_button": True,
    "navbar_end": ["theme-switcher", "navbar-icon-links"],
    "navbar_align": "content",
    "icon_links": [
        {
            "name": "GitHub",
            "url": "https://github.com/MetOffice/vernier",
            "icon": "fa-brands fa-github"
        }
    ],
    "secondary_sidebar_items": {
        "**/*": ["page-toc", "edit-this-page"],
        "index": [],
    },
    "footer_start": ["crown-copyright"],
    "footer_center": ["sphinx-version"],
    "footer_end": ["theme-version"]
}

html_sidebars = {
    "index": []
}

# Provides the Edit on GitHub link in the generated docs.
html_context = {
    "display_github": True,
    "github_user": "MetOffice",
    "github_repo": "vernier",
    "github_version": "main",
    "doc_path": "/documentation/Sphinx/"
}

html_permalinks = False
