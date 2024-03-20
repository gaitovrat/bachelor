FROM --platform=linux/amd64 ubuntu:latest

# Install Qt6
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y texlive-latex-extra \
	texlive-lang-czechslovak \
	texlive-bibtex-extra \
	git
