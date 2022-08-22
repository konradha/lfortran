import datetime
from json import load, dump
import os
import sys

dest_dir = sys.argv[1]
version = sys.argv[2]
lfortran_commit_sha = sys.argv[3]

filename = "data.json"
if os.path.exists(filename):
    d = load(open(filename))
else:
    d = {"dev": [], "release": []}
entry = {
    "url": "https://lfortran.github.io/wasm_builds/%s/%s" % \
            (dest_dir, lfortran_commit_sha),
    "version": version,
    "lfortran_commit_sha": lfortran_commit_sha,
    "created": str(datetime.datetime.now())
}
d[dest_dir].insert(0, entry)
print("Saving to %s." % filename)
with open(filename, "w") as f:
    dump(d, f, indent=4, ensure_ascii=False, sort_keys=True)
