#!/usr/bin/env python3

import pandas as pd
X = pd.read_csv('spreadsheet.txt', sep="\t", header=None)


X.columns = ['runno', 'baseline', 'gain', 'peaktime', 'dac', 'configpath']

#print(X)

Y = pd.read_csv('runadcstats_trimmed.dat', sep='\s+', header=None)
Y.columns = ['runno', 'indmin', 'sigma_indmin', 'indmax', 'sigma_indmax', 'cmin', 'sigma_cmin', 'cmax', 'sigma_cmax' ]

#print(Y)

# stupid merge by looping over dataframes -- should be a map

X.reset_index()
for index, row in X.iterrows():
    Y.reset_index()
    found = 0
    for index2, row2 in Y.iterrows():
        if (row['runno'] == row2['runno']):
            print(row['runno'], row['baseline'], row['gain'], row['peaktime'], row['dac'], row['configpath'], row2['indmin'], row2['sigma_indmin'], row2['indmax'], row2['sigma_indmax'], row2['cmin'], row2['sigma_cmin'], row2['cmax'], row2['sigma_cmax'])
            found = 1
            break
    if (found == 0):
        print(row['runno'], row['baseline'], row['gain'], row['peaktime'], row['dac'], row['configpath'], -1, -1, -1, -1, -1, -1, -1, -1)
        
            
