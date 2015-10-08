# S. Sundaresh, ACM116 FA2014, Caltech
# Problem Set 5 Sample Problem # 2

# runs the Markov chain's transition matrix forward in time to 
# estimate the probability of a run of arrivals of length 15 or greater.

m = 1E3; # number of samples, must be >= 1 for this script to work right, m
n = 15; # min length of run we're looking for, n
ep = 0; # memory parameter, epsilon

# create 'markov chain' transition matrix (see solutions for details)
function TMat = createTransitionMatrix (n,p,ep)
    T0 = (1-p-ep)*ones(1,n);
    T0(n) = 0;
    T0(1) = 1-p+ep;
    
    TMat = zeros(n,n);
    
    TMat(1,:) = T0;
    for i = 2:n
        TMat(i,i-1) = p+ep;
    endfor
    
    TMat(2,1) = p-ep;
    TMat(n,n) = 1; 
endfunction

# starting state of the chain is '0 run', [1; 0; 0; ...]
# first sample is always memoryless; state now [1-p; p; 0; 0; 0; ... ]
function InitState = initializeState (n,p,ep)
    InitState = zeros(n,1); 
    InitState(1) = 1-p;
    InitState(2) = p;
endfunction

printf("\n\n\n");
# let user know input parameters
printf("Looking for the probability of a run of length >= %d \nin a coin-flipping sequence of length %d \n\n",n,m);

# independent, unbiased (IU) samples case
    p = 0.5; 
    ep = 0;
    printf("Independent, unbiased flips, chance of success %.2f%s and memory parameter %.1f%s \n",100*p,"%",100*ep,"%");
    INIT = initializeState(n,p,ep);
    T = createTransitionMatrix(n,p,ep);
    
    # probability distribution for state at mth sample
    FINAL_STATE = (T^(m-1))*INIT;
    printf("Percent chance we saw a run of length >= %d is %.3f%s \n",n,100*FINAL_STATE(n),"%");
    

# dependent, unbiased (DU) samples case
    p = 0.5;
    ep = 0.01;
    printf("Dependent, unbiased flips, chance of success %.2f%s and memory parameter %.1f%s \n",100*p,"%",100*ep,"%");
    INIT = initializeState(n,p,ep);
    T = createTransitionMatrix(n,p,ep);
    
    # probability distribution for state at mth sample
    FINAL_STATE = (T^(m-1))*INIT;
    printf("Percent chance we saw a run of length >= %d is %.3f%s \n",n,100*FINAL_STATE(n),"%");
    
# independent, biased (IB) samples case
    p = 0.5092;
    ep = 0;
    printf("Independent, biased flips, chance of success %.2f%s and memory parameter %.1f%s \n",100*p,"%",100*ep,"%");
    INIT = initializeState(n,p,ep);
    T = createTransitionMatrix(n,p,ep);
    
    # probability distribution for state at mth sample
    FINAL_STATE = (T^(m-1))*INIT;
    printf("Percent chance we saw a run of length >= %d is %.3f%s \n",n,100*FINAL_STATE(n),"%");