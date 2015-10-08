# S. Sundaresh, ACM116 FA2014, Caltech
# Problem Set 5 Sample Problem # 1c 

# This script generates sample data for problem 1c and 
# estimates the parameter distribution for the generated data
# using Bayesian inference.

# Run this script on a local GNU Octave distribution or on octave-online.net; log in using google accounts.

# Simulate Markov Chain; how I generated the data.
p = 0.5;
ep = 0.01;
M = 10000;
# Packet = 1, NoPacket = 0
allStates = zeros(1,M);
for i = 1:M
    u = unifrnd(0,1);
    if i == 1 
        if u <= p
            allStates(i) = 1;
        else
            allStates(i) = 0;
        endif
    else
        if allStates(i-1) == 1
            if u <= p + ep
                # P(R_k|R_k-1,M) = p+ep
                allStates(i) = 1;
            else
                # P(W_k|R_k-1,M) = (1-p)-ep
                allStates(i) = 0;
            endif
        else
            if u <= p - ep
                # P(R_k|W_k-1,M) = p-ep
                allStates(i) = 1;
            else
                # P(W_k|W_k-1,M) = (1-p)+ep
                allStates(i) = 0;
            endif
        endif
    endif
endfor

# mean(allStates(1:M)) # approximately 0.5 as desired
# now generate relevant statistics [init, rr, rw, wr, ww]

rr = 0;
rw = 0;
wr = 0;
ww = 0;
init = 0;
if allStates(1) == 1
    init = 1;
endif
for i = 2:M
    if (allStates(i) == 1)
        if (allStates(i-1) == 1)
            rr = rr + 1;
        else
            rw = rw + 1;
        endif
    else
        if (allStates(i-1) == 1)
            wr = wr + 1;
        else
            ww = ww + 1;
        endif
    endif
endfor


#### Problem 1c ####
Np = 50;    # simulate a 50x20 grid of p,e values.
Ne = 20;
lls = zeros(Np,Ne);     # log likelihood over grid
pVals = zeros(Np,Ne);   # p parameter value at that grid element
eVals = zeros(Np,Ne);   # e parameter value at that grid element
p1 = linspace(0.1,0.9,Np); # span wide range of p
e1 = linspace(0.000,0.013,Ne);  # span tiny range of epsilon 
                                #(you'll come to this conclusion yourself 
                                # after some toying)
for ip = 1:Np
    for ie = 1:Ne
        pVals(ip,ie) = p1(ip);
        eVals(ip,ie) = e1(ie);  # p,e at this grid element selected
        t = 0;
        if init == 1
            t = t + log(p);
        else
            t = t + log(1-p);   # is our first datum an arrival or not?
        endif
        t = t + rr*log(p1(ip)+e1(ie)); 
        t = t + rw*log(p1(ip)-e1(ie));
        t = t + wr*log(1-p1(ip)-e1(ie));
        t = t + ww*log(1-p1(ip)+e1(ie));
        lls(ip,ie) = t;         # lls calculated as in Problem 1b.
    endfor
endfor

# now find the maximum of the distribution
# when you start out, with a coarse mesh, 
# it will look like it's (0.5,0) = (p,e)
# but as your mesh gets refined,
# you'll see the peak move away from e=0.
maxp = 0;
maxe = 0;
maxll = min(min(lls));
for ip = 1:Np
    for ie = 1:Ne
        if lls(ip,ie) >= maxll
            maxll = lls(ip,ie);
            maxp = ip;
            maxe = ie;
        endif
    endfor
endfor

tx = p1;
ty = e1;
[xx, yy] = meshgrid (tx, ty); 
tz = transpose(lls);
meshc(tx, ty, tz); # let's get a look at this distribution.

# compute local derivatives to get a feeling for how high the peak is
[maxll, p1(maxp), e1(maxe)] # maximum log likelihood 

# derivative in p direction from max
pderiv = (maxll - lls(maxp-1, maxe)) / (p1(maxp) - p1(maxp-1)); 
# derivative in e direction from max
ederiv = (maxll - lls(maxp, maxe-1)) / (e1(maxe) - e1(maxe-1)); 
# loglikelihood difference between this max and the (p = 0.5, e = 0) model.
maxll -  lls(26,1) 

pderiv, ederiv

view(90,0)
# isolating the relevant part, for a generated data set
axis([0 1 0 0.02 -7000 -6920]) 
# axis auto # for you to play with scaling yourself.
