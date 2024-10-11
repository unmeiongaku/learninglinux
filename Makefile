.PHONY: status add upload commit all

COMMIT_MSG ?= "first commit"

status:
	git status
add: 
	git add .
commit:
	git commit -m "$(COMMIT_MSG)"
upload:
	git push origin master
all:
	git status
	git add .
	git commit -m "$(COMMIT_MSG)"
	git push origin master
