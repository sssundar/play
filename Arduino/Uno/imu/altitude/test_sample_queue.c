/**
  * @file test_sample_queue.c
  * @author Sushant Sundaresh
  * @date 2016-12-04
  * @details
  * Tests the sample queue and data accessors in isolation prior 
  * to on-device testing.
  */

#include "test_helpers.h"
#include "data.h"
#include "timer_driver.h"
#include "sample_queue.h"

/* Unit under test */
static sSampleQueue uut;
static sData test_data, observed_data;
static uint8_t test_bytes[MAX_DATA_BYTES];
static sTicks test_clock;

/* Output Parsing */

static void clear_test_clock_ (void) {
    int k;
    for (k = 0; k < CLOCK_BYTE_WIDTH; k++) {
        test_clock.count[k] = 0;
    }
}

static void set_test_clock_ (uint8_t msb, uint8_t misb, uint8_t lsb) {
    test_clock.count[0] = msb;
    test_clock.count[1] = misb;
    test_clock.count[2] = lsb;
}

static void set_test_bytes_ (uint8_t msb, uint8_t misb, uint8_t lsb) {
    test_bytes[0] = msb;
    test_bytes[1] = misb;
    test_bytes[2] = lsb;
}

static bool create_sample_(sData *data, eDataType type, uint8_t *bytes, uint8_t num_bytes, sTicks *timestamp) {
    bool success = true;
    success = success & (ksuccess == data_set_type(data, type));
    int k;
    for (k = 0; k < num_bytes; k++) {
        success = success && (ksuccess == data_add_byte(data, bytes[k]));
    }
    success = success && (ksuccess == data_set_ticks(data,timestamp));
    return success;
}

static void clear_sample_(sData *data) {
    data_set_type(data, kdatatype_unset);    
}

static bool compare_samples_(sData *a, sData *b) {    
    bool types, bytes, ticks;

    eDataType atype = a->type;
    eDataType btype = b->type;
    types = atype == btype;
    
    uint8_t abytes[MAX_DATA_BYTES]; uint8_t alen;
    uint8_t bbytes[MAX_DATA_BYTES]; uint8_t blen;
    data_get_bytes(a,abytes,&alen);
    data_get_bytes(b,bbytes,&blen);
    bytes = (alen == blen);
    int k;
    if (bytes) {        
        for (k = 0; k < alen; k++) {
            bytes = bytes && (abytes[k] == bbytes[k]);
        }
    }

    sTicks aticks, bticks;
    data_get_ticks(a, &aticks);
    data_get_ticks(b, &bticks); 
    ticks = true;
    for (k = CLOCK_BYTE_WIDTH-1; k > -1; k--) {
        ticks = ticks && (aticks.count[k] == bticks.count[k]);
    }  

    return types && bytes && ticks;
}

/* Unit Test */
int main (int argc, char **argv) {    
    (void) argc; (void) argv;
    clear_test_clock_();

    /* Vector 0: Queue Initialization */
    if (!check(kerror == sampleq_init(NULL))) { return 1; }
    if (!check(ksuccess == sampleq_init(&uut))) { return 1; }
    increment_test_vector();

    /* Vector 1: Data Size Checking */
    if (!check(create_sample_(&test_data, kdatatype_unset, test_bytes, 0, &test_clock))) { return 1; }
    if (!check(!create_sample_(&test_data, kdatatype_unset, test_bytes, 1, &test_clock))) { return 1; }    
    increment_test_vector();

    /* Vector 2: Data bytes and clock setting, and empty queue enq and deq */    
    set_test_bytes_(4,5,6);
    set_test_clock_(3,2,1);
    if (!check(create_sample_(&test_data, kdatatype_bmp180_barometry, test_bytes, 3, &test_clock))) { return 1; }
    clear_sample_(&observed_data);    
    if (!check(kfailure == sampleq_dequeue(&uut, &observed_data))) { return 1; }   
    if (!check(ksuccess == sampleq_enqueue(&uut, &test_data))) { return 1; }       
    if (!check(ksuccess == sampleq_dequeue(&uut, &observed_data))) { return 1; }   
    if (!check(compare_samples_(&test_data, &observed_data))) { return 1; }
    increment_test_vector();   

    /* Vector 3: Test full and empty conditions */
    sampleq_init(&uut);
    int k = 0;
    do {                
        set_test_bytes_(k+2,k+1,k+0);
        set_test_clock_(k+0,k+1,k+2);
        create_sample_(&test_data, kdatatype_bmp180_barometry, test_bytes, 3, &test_clock);
        k++;        
    } while (sampleq_enqueue(&uut, &test_data) == ksuccess);
        
    if (!check(SAMPLE_QUEUE_LENGTH+1 == k)) { return 1; }

    k = 0;
    while (sampleq_dequeue(&uut, &observed_data) == ksuccess) {
        set_test_bytes_(k+2,k+1,k+0);
        set_test_clock_(k+0,k+1,k+2);
        create_sample_(&test_data, kdatatype_bmp180_barometry, test_bytes, 3, &test_clock);        
        if (!check(compare_samples_(&test_data, &observed_data))) { return 1; }
        k++;
    }                    

    if (!check(SAMPLE_QUEUE_LENGTH == k)) { return 1; }
    increment_test_vector();   

    /* Vector 4: Test wraparound */
    sampleq_init(&uut);
    set_test_clock_(0,0,0);
    k = 0;
    do {
        set_test_bytes_(0,0,k);     
        create_sample_(&test_data, kdatatype_bmp180_barometry, test_bytes, 3, &test_clock);        
        k++;
    } while (sampleq_enqueue(&uut, &test_data) == ksuccess);
    sampleq_dequeue(&uut, &observed_data);
    sampleq_dequeue(&uut, &observed_data);
    do {
        set_test_bytes_(0,0,k);     
        create_sample_(&test_data, kdatatype_bmp180_barometry, test_bytes, 3, &test_clock);
        k++;
    } while (sampleq_enqueue(&uut, &test_data) == ksuccess);

    if (!check(uut.buffer[0].bytes[2] == k-2)) { return 1; }    
    increment_test_vector();   

    /* Vector 5: Write then read a sample with less than the maximum number of bytes */
    set_test_bytes_(0,0,0);     
    create_sample_(&test_data, kdatatype_bmp180_barometry, test_bytes, 3, &test_clock);            
    set_test_bytes_(1,2,3);
    create_sample_(&test_data, kdatatype_bmp180_thermometry, test_bytes, 2, &test_clock);            
    if (!check(test_data.bytes[0] == 1)) { return 1; }
    if (!check(test_data.bytes[1] == 2)) { return 1; }
    if (!check(test_data.bytes[2] == 0)) { return 1; }
    increment_test_vector();   

    return 0;
}