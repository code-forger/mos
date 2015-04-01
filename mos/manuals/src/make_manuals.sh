pdflatex -interaction=batchmode user_manual.tex
pdflatex -interaction=batchmode programmer_manual.tex
pdflatex -interaction=batchmode installer_manual.tex

cp *.pdf ../