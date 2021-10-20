
RAWFILES=$(wildcard *dl1.root)

DECODEFILES=$(RAWFILES:dl1.root=dl1_decode.root)

FITTXTFILES=$(DECODEFILES:.root=.root_fits.txt)

FITNTUPLES=$(FITTXTFILES:.txt=.txt_ntuple.root)

CHANSUMTXTFILES=$(FITNTUPLES:ntuple.root=ntuple.root_chans.txt)

CHANSUMNTUPLEFILES=$(CHANSUMTXTFILES:chans.txt=chans.txt_ntuple.root)

decode : $(DECODEFILES)

fittxt : $(FITTXTFILES)

fitntuple : $(FITNTUPLES)

chansumtxt : $(CHANSUMTXTFILES)

chansumntuple : $(CHANSUMNTUPLEFILES) 

%dl1_decode.root : %dl1.root
	-lar -c RunIcebergRawDecoder14.fcl -T /dev/null $<

%decode.root_fits.txt : %decode.root
	-./fitpulserfile.sh $<

%fits.txt_ntuple.root : %fits.txt
	-./makentuplefile.sh $<

%.root_fits.txt_ntuple.root_chans.txt : %.root_fits.txt_ntuple.root
	-./runsummaryfile.sh $<

%_chans.txt_ntuple.root : %_chans.txt
	-./summaryntuplefile.sh $<
