ea - Character Encoding Analyzer

Analyze a text file or stream for the following character encoding categories

- CONTROL characters
- ASCII encoded characters
- UNICODE encoded characters
- Characters encoded in a fallback encoding
- Characters with UNKNOWN encoding

- Print file content with (optionally) different colors for each category.
- Optionally print character count for each category
- Optionally print character count for each character in each category

By applying a filter output is only processed for a subset of categories.
Available categories:

    control,ascii,unicode,fallback,unknown

The first two characters of a category are sufficient. For example to filter
output for ASCII, UNICODE and fallback encoding, use --filter as,fa,un.
