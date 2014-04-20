#!/bin/bash

# Docs for environment variables are found in Targeting_Mac_OS.pdf
# Page 345-350

# Check the file out with perforce

"${SDKS}/macosx/bin/p4" edit "${SDKS}/mac/burgerbase/$MW_OUTPUT_NAME"

# Copy the output file into the final location

cp "$MW_OUTPUT_FILE" "${SDKS}/mac/burgerbase/$MW_OUTPUT_NAME"
