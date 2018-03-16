# scorpy-extra

Extra utils and stuff for Scorpy ( https://github.com/majava3000/scorpy )

# License

All code licensed under the GPLv2

# Tools

## binfilter

Simple C program to filter through Saleae Logic binary changed based export
files (digital only). Emits a new file which will only contain changes selected
with a 16-bit channel hex mask. This is mostly useful to avoid slow python
ingesting of such files when working with Scorpy.
