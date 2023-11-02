from setuptools import setup, find_packages

setup(
    name="AnalogInformationDecoding",
    version="0.1",
    packages=find_packages(),
    install_requires=[
        "numpy",
        "scipy",
        "numba",
        "matplotlib",
        "bposd",
        "ldpc",
    ],
    author="Lucas Berent, Timo Hillmann",
    author_email="lucas.berent@tum.de, timo.hillmann@rwth-aachen.de",
    description="Analog Information Decoding techniques",
    license="MIT",
    keywords="bosonic decoders",
    url="",
    )