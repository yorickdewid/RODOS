#!/usr/bin/env python

import sys
import os
import time

try:
	import jinja2
except ImportError:
	print "To use this functionality you need to install the jinja2 template engine (easy_install jinja2)"
	sys.exit(1)

globals = {
		'time': time.strftime("%d %b %Y, %H:%M:%S", time.localtime()),
}

def template(infile, outfile, substitutions):
	path, file = os.path.split(infile)
	loader = jinja2.Environment(loader = jinja2.FileSystemLoader(path))
	template = loader.get_template(file, globals=globals)
	
	output = template.render(substitutions)
	open(outfile, 'w').write(output)

if __name__ == '__main__':
	substitutions = {
		'id': 0
	}
	
	for id in range(0, 4):
		substitutions['id'] = id
		template("uart.cpp.in",	"uart_%i.cpp" % id,	substitutions)
		template("uart.h.in",	"uart_%i.h" % id,	substitutions)