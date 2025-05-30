"""Sphinx documentation configuration file."""

from datetime import datetime
import os
import pathlib

ROOT_DIR = pathlib.Path(__file__).parent.parent.parent

# -- Project information -----------------------------------------------------

project = "ansys-tools-filetransfer-server"
copyright = f"(c) {datetime.now().year} ANSYS, Inc. All rights reserved"
author = "ANSYS Inc."

# Read version from VERSION file
with open(os.path.join("..", "..", "VERSION"), "r") as f:
    version_file = f.readline().strip()

release = version = version_file

# -- General configuration ---------------------------------------------------
extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.coverage",
    "sphinx.ext.extlinks",
    "sphinx.ext.intersphinx",
    "sphinx_copybutton",
    "sphinx.ext.mathjax",
    "sphinx.ext.viewcode",
    "sphinx.ext.imgmath",
    "sphinx.ext.todo",
    "breathe",
    "sphinx_design",
]

# Intersphinx mapping
intersphinx_mapping = {
    # "python": ("https://docs.python.org/dev", None),
    # "scipy": ("https://docs.scipy.org/doc/scipy/reference", None),
    # "numpy": ("https://numpy.org/devdocs", None),
    # "matplotlib": ("https://matplotlib.org/stable", None),
    # "pandas": ("https://pandas.pydata.org/pandas-docs/stable", None),
    # "pyvista": ("https://docs.pyvista.org/", None),
}

# The suffix(es) of source filenames.
source_suffix = ".rst"

# The master toctree document.
master_doc = "index"

# The language for content autogenerated by Sphinx. Refer to documentation
# for a list of supported languages.
#
# This is also used if you do content translation via gettext catalogs.
# Usually you set "language" from the command line for these cases.
language = "en"

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ["_build"]

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = "sphinx"

# If true, `todo` and `todoList` produce output, else they produce nothing.
todo_include_todos = False

# Copy button customization ---------------------------------------------------
# exclude traditional Python prompts from the copied code
copybutton_prompt_text = r">>> ?|\.\.\. "
copybutton_prompt_is_regexp = True


# -- Options for HTML output -------------------------------------------------
html_short_title = html_title = "Filetransfer Tool Server"
html_theme = "ansys_sphinx_theme"
html_theme_options = {
    "logo": "ansys",
    "github_url": "https://github.com/ansys/ansys-tools-filetransfer-server",
    "show_prev_next": True,
    "show_breadcrumbs": True,
    "additional_breadcrumbs": [
        ("PyAnsys", "https://docs.pyansys.com/"),
    ],
}

# -- Options for HTMLHelp output ---------------------------------------------

# Output file base name for HTML help builder.
htmlhelp_basename = "ansys-tools-filetransfer-server-doc"


# -- Options for LaTeX output ------------------------------------------------
latex_elements = {}

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title,
#  author, documentclass [howto, manual, or own class]).
latex_documents = [
    (
        master_doc,
        f"ansys-tools-filetransfer-server.tex",
        "Filetransfer Tool Server Documentation",
        author,
        "manual",
    ),
]


# -- Options for manual page output ------------------------------------------

# One entry per manual page. List of tuples
# (source start file, name, description, authors, manual section).
man_pages = [
    (
        master_doc,
        "ansys-tools-filetransfer-server",
        "Filetransfer Tool ServerDocumentation",
        [author],
        1,
    )
]


# -- Options for Texinfo output ----------------------------------------------

# Grouping the document tree into Texinfo files. List of tuples
# (source start file, target name, title, author,
#  dir menu entry, description, category)
texinfo_documents = [
    (
        master_doc,
        "ansys-tools-filetransfer-server",
        "Filetransfer Tool Server Documentation",
        author,
        "ansys-tools-filetransfer-server",
        "A demo project for showing different protocol communications (API REST, gRPC) both in Python and C++.",
        "Engineering Software",
    ),
]


# -- Options for Epub output -------------------------------------------------

# Bibliographic Dublin Core info.
epub_title = project

# The unique identifier of the text. This can be a ISBN number
# or the project homepage.
#
# epub_identifier = ''

# A unique identification for the text.
#
# epub_uid = ''

# A list of files that should not be packed into the epub file.
epub_exclude_files = ["search.html"]

# -- Import the C++ docs -----------------------------------------------------

# import subprocess

# subprocess.check_call(["make", "clean"], shell=True, cwd=ROOT_DIR / "doc")
# subprocess.check_call("doxygen", cwd=ROOT_DIR / "doxygen")

breathe_projects = {
    "ansys-tools-filetransfer-server": "../doxygen/xml/",
}
breathe_default_project = "ansys-tools-filetransfer-server"

breathe_projects_source = {
    "ansys-tools-filetransfer-server": (
        "../../src",
        ["server.cpp", "lib/"],
    ),
}
breathe_show_include = False
breathe_separate_member_pages = True
