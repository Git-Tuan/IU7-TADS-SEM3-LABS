#!/usr/bin/env bash

create()
{
    touch "$1";
    chmod 755 "$1"
}

for file in "$@"; do
    create "$file"
    echo "#!/usr/bin/env bash" > "$file"
done
