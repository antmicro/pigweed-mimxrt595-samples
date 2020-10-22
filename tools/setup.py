"""Utilities for Sample Project development."""

import setuptools  # type: ignore

setuptools.setup(
    name='sample_project_tools',
    version='0.0.1',
    description=__doc__,
    packages=setuptools.find_packages(),
    install_requires=[],
    package_data={'sample_project_tools': ['py.typed']},
    zip_safe=False,
)
