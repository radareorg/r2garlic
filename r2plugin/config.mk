EXTERNAL_PLUGINS+=r2garlic

.PHONY: r2garlic

r2garlic: p/r2garlic

p/r2garlic:
	cd p && git clone https://github.com/neocanable/r2garlic
