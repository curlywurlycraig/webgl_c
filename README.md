# Copy template html

`cp template.html build/index.html`

# Building

`emcc main.c -o build/ -s FULL_ES2=1`

# Serving

Use something like `miniserve build/` to serve.
