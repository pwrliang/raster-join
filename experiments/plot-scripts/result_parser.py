import re

def readResults(filename):
    f = open(filename , 'r')
    #Result: ptsSize, polySize, function, noPtPasses, noConstraints, executeTime, ptMemTime, ptRenderTime, polyMemTime, polyRenderTime, setupTime, triangulationTime, polyIndexTime, backendQueryTime, accuracy 
    # 226258216		16384	0	1	0	26816	0	26176	2	5	4	51	0	0	100
    converter = (int, int, int, int, int, int, int, int, int, int, int, int, int, int)
    converter_accuracy = (int, int, int, int, int, int, int, int, int, int, int, int, int, int, int)
    results = []
    for line in f:
    	if not line in ['\n', '\r\n']:
        	fields = re.split(r'\s+', line.strip())
        	length = len(fields)
        	if length == 14:
        		record = tuple([converter[i](fields[i]) for i in xrange(length)])
        	elif length == 15:
        		record = tuple([converter_accuracy[i](fields[i]) for i in xrange(length)])
        	results.append(record)   
    return results