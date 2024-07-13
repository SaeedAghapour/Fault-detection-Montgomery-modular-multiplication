#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <math.h>
#include <time.h>
#include <unistd.h>


int calculateBitSize(const mpz_t x) 
{
    int bitSize = 0;
    mpz_t temp;
    mpz_init(temp);
    mpz_set(temp, x);

    while (mpz_cmp_ui(temp, 0) > 0) 
    {
        mpz_tdiv_q_2exp(temp, temp, 1);
        bitSize++;
    }
    mpz_clear(temp);
    return bitSize;
}


gmp_randstate_t rand_state;

void grand(const mpz_t range, mpz_t rand_num)
{
    if (mpz_cmp_ui(range, 0) > 0) 
    {
        mpz_urandomm(rand_num, rand_state, range);
    } 
    else 
    {
        printf("Error: range must be greater than zero.\n");
    }
}

// Function to perform Montgomery reduction
void montgomery_mult(mpz_t result, mpz_t partial_result1, mpz_t weight, mpz_t u, mpz_t v, mpz_t N, int n, int word_size, const mpz_t b, const mpz_t N_prime, int l1, int *fault_t_counter) {
    mpz_t mi, v0, t, t_old, right_t, left_t, ui, t0, temp1, temp2, pow2;
    mpz_t fault_t, range_fault_t;

    
    mpz_inits(mi, v0, t, t_old, right_t, left_t, ui, t0, temp1, temp2, fault_t, range_fault_t, pow2, NULL);
    
    *fault_t_counter = 0;

    int size_v = mpz_sizeinbase(v, 2);
    int size_t = n * word_size;
/**************************************************************************************************************************/

    int fault_index_t;
    mpz_set_ui(fault_t, 0);

    // fault injections:

    int model_loop; // Scenario 1
    // ********************************** fault injections on ui **********************************
    /**********************************************************************************************/
    // fault model_loop: 1 => random bit flipping for ui. At n0 random iterations we will flip n1 bits of ui.
    // fault model_loop: 2 => random stuck at 1. At n0 random iterations we will replace n1 bits of ui with 1.
    // fault model_loop: 3 => burst bit flipping on ui. Starting from a random iteration, for the next n0 consequtive iterations we will flip n1 bits of ui.
    // fault model_loop: 4 => burst stuck at 1 on ui. Starting from a random iteration, for the next n0 consequtive iterations we will replace n1 bits of ui with 1.

    // ********************************** fault injections on mi **********************************
    /**********************************************************************************************/
    // fault model_loop: 5 => random bit flipping for mi. At n0 random iterations we will flip n1 bits of mi.
    // fault model_loop: 6 => random stuck at 1. At n0 random iterations we will replace n1 bits of mi with 1.
    // fault model_loop: 7 => burst bit flipping on mi. Starting from a random iteration, for the next n0 consequtive iterations we will flip n1 bits of mi.
    // fault model_loop: 8 => burst stuck at 1 on mi. Starting from a random iteration, for the next n0 consequtive iterations we will replace n1 bits of mi with 1.
    
    // ********************************** fault injections on v **********************************
    /**********************************************************************************************/
    // fault model_loop: 9 => random bit flipping for v. At n0 random iterations we will flip n1 bits of v.
    // fault model_loop: 10 => random stuck at 1. At n0 random iterations we will replace n1 bits of v with 1.
    // fault model_loop: 11 => burst bit flipping on v. Starting from a random iteration, for the next n0 consequtive iterations we will flip n1 bits of v.
    // fault model_loop: 12 => burst stuck at 1 on v. Starting from a random iteration, for the next n0 consequtive iterations we will replace n1 bits of v with 1.
    
    // ********************************** fault injections on t **********************************
    /**********************************************************************************************/
    // fault model_loop: 13 => random bit flipping for t. At n0 random iterations we will flip n1 bits of t.
    // fault model_loop: 14 => random stuck at 1. At n0 random iterations we will replace n1 bits of t with 1.
    // fault model_loop: 15 => burst bit flipping on t. Starting from a random iteration, for the next n0 consequtive iterations we will flip n1 bits of t.
    // fault model_loop: 16 => burst stuck at 1 on t. Starting from a random iteration, for the next n0 consequtive iterations we will replace n1 bits of t with 1.
    
    model_loop = 16;

    int n0 = 0;            // n0 determines the number of faulty iterations.
    int n1 = 0;            // n1 determines the number of fault injections in each faulty iteration

    int start = (random() * 7919) % (n + 1); // starting point for burst
    int burst_range_start;
    int burst_range_end;

    unsigned long long *random_faults = (unsigned long long *)malloc(n0 * sizeof(unsigned long long));

    // Generate and store random numbers
    for (int i = 0; i < n0; i++) 
    {
        random_faults[i] = (random() * 7919) % (n + 1); // random iterations 
    }

    int shift_factor;
    mpz_set_ui(t, 0);
    mpz_mod(v0, v, b);

    mpz_set_ui(t_old, 0);
    mpz_set_ui(weight, 0);


// main loop of the Montgomery algorithm
    for (int i = 0; i < n; i++) {

    // ui
        shift_factor = word_size * i;
        mpz_tdiv_q_2exp(ui, u, shift_factor);
        mpz_mod(ui, ui, b);

///////////////////////////////// fault on ui- size of ui is word_size //////////////////// model 1, 2, 3, and 4

/////// Model: Random

        // 1: type: Flipping
        if (model_loop == 1) // random flipping. at n0 random iterations we flip n1 random bits of ui 
        {
           // gmp_printf(" ui: %Zd\n", digit);
            for (int j = 0; j < n0; j ++)
            {
                if (random_faults[j] == i)
                {
                    for(int k = 0; k < n1; k++) // how many bits of t should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (n + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_xor(ui, ui, pow2);
                    }
                }
            }
        }

        // 2: type: Stuck at 1
        if (model_loop == 2) // random stuck at 1 at n0 random iterations we stuck at 1 n1 random bits of ui
        {
           // gmp_printf(" ui: %Zd\n", digit);
            for (int j = 0; j < n0; j ++)
            {
                if (random_faults[j] == i)
                {
                    for(int k = 0; k < n1; k++) // how many bits of t should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (n + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_ior(ui, ui, pow2);
                    }
                }
            }
        }

/////// Model: Burst

        // 3: type: Flipping
        if (model_loop == 3) // burst flipping on ui. starting at one random bit (start) we change the value of ui for the next n0 consecutive iterations (in each iterations n1 bits are flipped)
        {
            if (start + n0 < n) // the last burst injection is not reaching n
            {
                if(start <= i && i < start + n0)
                {
                    for(int k = 0; k < n1; k++) // how many bits of t should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (n + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_xor(ui, ui, pow2);
                    }
                }
            }

            else if (start + n0 >= n)
            {
                if(i < (start + n0) % n || i >= start ) // the last burst injection has surpassed n
                {
                    for(int k = 0; k < n1; k++) // how many bits of t should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (n + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_xor(ui, ui, pow2);
                    }
                }
            }
        }

        // 4: type: Stuck at 1
        if (model_loop == 4) // burst stuck at 1 on ui. 
        {
            if (start + n0 < n) // the last burst injection is not reaching n
            {
                if(start <= i && i < start + n0)
                {
                    for(int k = 0; k < n1; k++) // how many bits of t should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (n + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_ior(ui, ui, pow2);
                    }
                }
            }

            else if (start + n0 >= n)
            {
                if(i < (start + n0) % n || i >= start ) // the last burst injection has surpassed n
                {
                    for(int k = 0; k < n1; k++) // how many bits of t should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (n + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_ior(ui, ui, pow2);
                    }
                }
            }
        }

        mpz_add(weight, weight, ui);

    // temp1 = uiv0
        mpz_mul(temp1, ui, v0);

    //m_i = temp2 = (t0 + uiv0)N'%b
        mpz_mod(t0, t, b);
        mpz_add(temp2, t0, temp1);
        mpz_mul(temp2, temp2, N_prime);
        mpz_mod(mi, temp2, b); 



///////////////////////////////// fault on mi- size of mi is word_size //////////////////// model 5, 6, 7, and 8

/////// Model: Random

        // 5: type: Flipping
        if (model_loop == 5) // random flipping. at n0 random iterations we flip n1 random bits of mi 
        {
            for (int j = 0; j < n0; j ++)
            {
                if (random_faults[j] == i)
                {
                    for(int k = 0; k < n1; k++) // how many bits of t should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (n + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_xor(mi, mi, pow2);
                    }
                }
            }
        }

        // 6: type: Stuck at 1
        if (model_loop == 6) // random stuck at 1 at n0 random iterations we stuck at 1 n1 random bits of mi
        {
            for (int j = 0; j < n0; j ++)
            {
                if (random_faults[j] == i)
                {
                    for(int k = 0; k < n1; k++) // how many bits of t should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (n + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_ior(mi, mi, pow2);
                    }
                }
            }
        }

/////// Model: Burst

        // 7: type: Flipping
        if (model_loop == 7) // burst flipping on mi. starting at one random bit (start) we change the value of ui for the next n0 consecutive iterations (in each iterations n1 bits are flipped)
        {
            if (start + n0 < n) // the last burst injection is not reaching n
            {
                if(start <= i && i < start + n0)
                {
                    for(int k = 0; k < n1; k++) // how many bits of t should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (n + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_xor(mi, mi, pow2);
                    }
                }
            }

            else if (start + n0 >= n)
            {
                if(i < (start + n0) % n || i >= start ) // the last burst injection has surpassed n
                {
                //counter++;
                    for(int k = 0; k < n1; k++) // how many bits of t should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (n + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_xor(mi, mi, pow2);
                    }
                }
            }
        }

        // 8: type: Stuck at 1
        if (model_loop == 8) // burst stuck at 1 on mi. 
        {
            if (start + n0 < n) // the last burst injection is not reaching n
            {
                if(start <= i && i < start + n0)
                {
                    for(int k = 0; k < n1; k++) // how many bits of t should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (n + 1);
                        //  printf("fault_index %d\n", fault_index_t);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_ior(mi, mi, pow2);
                    }
                }
            }

            else if (start + n0 >= n)
            {
                if(i < (start + n0) % n || i >= start ) // the last burst injection has surpassed n
                {
                //counter++;
                    for(int k = 0; k < n1; k++) // how many bits of t should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (n + 1);
                        //  printf("fault_index %d\n", fault_index_t);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_ior(mi, mi, pow2);
                    }
                }
            }
        }



    // t = (t + uiv + miN) /b

///////////////////////////////// fault on v. v could be 2048 bit here //////////////////// model 9, 10, 11, and 12

/////// Model: Random

        // 9: type: Flipping
        if (model_loop == 9) // random flipping. at n0 random iterations we flip n1 random bits of v 
        {
            for (int j = 0; j < n0; j ++)
            {
                if (random_faults[j] == i)
                {
                    for(int k = 0; k < n1; k++) // how many bits of v should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (size_v + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_xor(v, v, pow2);
                    }
                }
            }
        }

        // 10: type: Stuck at 1
        if (model_loop == 10) // random stuck. at n0 random iterations we stick at 1 n1 random bits of v 
        {
            for (int j = 0; j < n0; j ++)
            {
                if (random_faults[j] == i)
                {
                    for(int k = 0; k < n1; k++) // how many bits of v should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (size_v + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_ior(v, v, pow2);
                    }
                }
            }
        }

/////// Model: Burst

        // 11: type: Flipping
        if (model_loop == 11) // burst flipping on v. starting at one random bit (start) we change the value of v for the next n0 consecutive iterations (in each iterations n1 bits are flipped)
        {
            if (start + n0 < n) // the last burst injection is not reaching n
            {
                if(start <= i && i < start + n0)
                {
                    for(int k = 0; k < n1; k++) // how many bits of v should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (size_v + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_xor(v, v, pow2);
                    }
                }
            }

            else if (start + n0 >= n)
            {
                if(i < (start + n0) % n || i >= start ) // the last burst injection has surpassed n
                {
                //counter++;
                    for(int k = 0; k < n1; k++) // how many bits of v should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (size_v + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_xor(v, v, pow2);
                    }
                }
            }
        }

        // 12: type: Stuck at 1
        if (model_loop == 12) // burst stuck at 1 on v. starting at one random bit (start) we change the value of v for the next n0 consecutive iterations (in each iterations n1 bits are stuck at 1)
        {
            if (start + n0 < n) // the last burst injection is not reaching n
            {
                if(start <= i && i < start + n0)
                {
                    for(int k = 0; k < n1; k++) // how many bits of v should we flip in each faulty iteration
                    {
                        fault_index_t = (random() * 7919) % (size_v + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_ior(v, v, pow2);
                    }
                }
            }

            else if (start + n0 >= n)
            {
                if(i < (start + n0) % n || i >= start ) // the last burst injection has surpassed n
                {
                //counter++;
                    for(int k = 0; k < n1; k++) // how many bits of t should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (size_v + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_ior(v, v, pow2);
                    }
                }
            }
        }


/*********************************************************************************************************************************/

    // temp1 = uiv
        mpz_mul(temp1, ui, v);

    // temp2 = miN
        mpz_mul(temp2, mi, N);

    //t + uiv + miN    
        mpz_add(t, t, temp1);
        mpz_add(t, t, temp2);
        mpz_tdiv_q_2exp(t, t, word_size); // right shift by 2^1 (equals to division by 2^1)
       // gmp_printf(" t/: %Zd\n", t);


///////////////////////////////// fault on t- t could be 2048 bit here //////////////////// model 13, 14, 15, and 16

/////// Model: Random

        // 13: type: Flipping
        if (model_loop == 13) // random flipping. at n0 random iterations we flip n1 random bits of t 
        {
            for (int j = 0; j < n0; j ++)
            {
                if (random_faults[j] == i)
                {
                    for(int k = 0; k < n1; k++) // how many bits of t should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (size_t + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_xor(t, t, pow2);
                    }
                }
            }
        }

        // 14: type: Stuck at 1
        if (model_loop == 14) // random stuck. at n0 random iterations we stick at 1 n1 random bits of t 
        {
            for (int j = 0; j < n0; j ++)
            {
                if (random_faults[j] == i)
                {
                    for(int k = 0; k < n1; k++) // how many bits of t should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (size_t + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_ior(t, t, pow2);
                    }
                }
            }
        }

/////// Model: Burst

        // 15: type: Flipping
        if (model_loop == 15) // burst flipping on t. starting at one random bit (start) we change the value of t for the next n0 consecutive iterations (in each iterations n1 bits are flipped)
        {
            if (start + n0 < n) // the last burst injection is not reaching n
            {
                if(start <= i && i < start + n0)
                {
                    for(int k = 0; k < n1; k++) // how many bits of t should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (size_t + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_xor(t, t, pow2);
                    }
                }
            }

            else if (start + n0 >= n)
            {
                if(i < (start + n0) % n || i >= start ) // the last burst injection has surpassed n
                {
                    for(int k = 0; k < n1; k++) // how many bits of t should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (size_t + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_xor(t, t, pow2);
                    }
                }
            }
        }

        // 16: type: Stuck at 1
        if (model_loop == 16) // burst stuck at 1 on t. starting at one random bit (start) we change the value of t for the next n0 consecutive iterations (in each iterations n1 bits are stuck at 1)
        {
            if (start + n0 < n) // the last burst injection is not reaching n
            {
                if(start <= i && i < start + n0)
                {
                    for(int k = 0; k < n1; k++) // how many bits of v should we flip in each faulty iteration
                    {
                        fault_index_t = (random() * 7919) % (size_t + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_ior(t, t, pow2);
                    }
                }
            }

            else if (start + n0 >= n)
            {
                if(i < (start + n0) % n || i >= start ) // the last burst injection has surpassed n
                {
                    for(int k = 0; k < n1; k++) // how many bits of t should we flip in each fauly iteration
                    {
                        fault_index_t = (random() * 7919) % (size_t + 1);
                        mpz_init_set_ui(pow2, 1);
                        mpz_mul_2exp(pow2, pow2, fault_index_t);
                        mpz_ior(t, t, pow2);
                    }
                }
            }
        }


    /**********************************************************************************************************************/
    // countermeasure 3

    mpz_add(right_t, temp1, temp2);
    mpz_add(right_t, right_t, t_old);

    mpz_mul_2exp(left_t, t, word_size); // bt

    if (mpz_cmp(left_t, right_t) == 0)
    {
        (*fault_t_counter) ++;
    }
    mpz_set(t_old, t);

    /**********************************************************************************************************************/
    //partial result 1

        if(i == l1-1)
        {
            mpz_set(partial_result1, t);    
        }
    }

    mpz_set(result, t);

    while (mpz_cmp(result, N) >= 0) {
        mpz_sub(result, result, N);
    }

    free(random_faults);
    mpz_clears(mi, v0, t, t_old, right_t, left_t, ui, t0, temp1, temp2, fault_t, range_fault_t, pow2, NULL);
}


void montgomery_recomputation(mpz_t partial_result1, mpz_t weight, mpz_t u, mpz_t v, mpz_t N, int n, int word_size, const mpz_t b, const mpz_t N_prime, int l1, int E_index_u , int E_index_v) {
    mpz_t mi, v0, t, t_old, right_t, left_t, ui, t0, temp1, temp2, pow2;
    mpz_t fault_t, range_fault_t;
    
    mpz_inits(mi, v0, t, t_old, right_t, left_t, ui, t0, temp1, temp2, fault_t, range_fault_t, pow2, NULL);
    

/**************************************************************************************************************************/
    int shift_factor;

    mpz_tdiv_q_2exp(u, u, E_index_u);
    mpz_tdiv_q_2exp(v, v, E_index_v);

    mpz_set_ui(t, 0);
    mpz_mod(v0, v, b);
    mpz_set_ui(weight, 0);

// main loop of the Montgomery algorithm
    for (int i = 0; i < l1; i++) {

    // ui
        shift_factor = word_size * i;
        mpz_tdiv_q_2exp(ui, u, shift_factor);
        mpz_mod(ui, ui, b);
        mpz_add(weight, weight, ui);

    // temp1 = uiv0
        mpz_mul(temp1, ui, v0);

    //m_i = temp2 = (t0 + uiv0)N'%b
        mpz_mod(t0, t, b);
        mpz_add(temp2, t0, temp1);
        mpz_mul(temp2, temp2, N_prime);
        mpz_mod(mi, temp2, b); 

    // t = (t + uiv + miN) /b

    // temp1 = uiv
        mpz_mul(temp1, ui, v);

    // temp2 = miN
        mpz_mul(temp2, mi, N);

    //t + uiv + miN    
        mpz_add(t, t, temp1);
        mpz_add(t, t, temp2);
        mpz_tdiv_q_2exp(t, t, word_size); // right shift by 2^1 (equals to division by 2^1)
    }
    mpz_set(partial_result1, t);

    for (int i = l1; i < n; i++) {
        shift_factor = word_size * i;
        mpz_tdiv_q_2exp(ui, u, shift_factor);
        mpz_mod(ui, ui, b);
        mpz_add(weight, weight, ui);
    }

    mpz_set_ui(t, 0);
    mpz_mod(v0, u, b); // v0 now is u0

/**********************************************************************************************************************/
//Computing partial result 2

    mpz_clears(mi, v0, t, t_old, right_t, left_t, ui, t0, temp1, temp2, fault_t, range_fault_t, pow2, NULL);
}

int main() {

    gmp_randinit_default(rand_state);
    gmp_randseed_ui(rand_state, time(NULL));
    srandom(time(NULL));

    mpz_t u, v; //inputs
    mpz_t u1, u2, v1, v2; //encoded inputs
    mpz_t u1f, u2f, v1f, v2f; //faulty inputs
    mpz_t range_flip, temp_mul, mask, z, range_u, range_v; //temporary regs
    mpz_t b, R, N, N_prime; //other params
    mpz_t weight1, weight2, answer, result, partial_result1, partial_result2; //outputs
    mpz_inits(weight1, weight2, u1, u2, v1, v2, u1f, u2f, v1f, v2f, range_flip, temp_mul, mask, z, b, R, N, N_prime, u, v, answer, result, partial_result1, partial_result2, range_u, range_v, NULL);

    int E_index_u, E_index_v; // encoded index

// fault counters
    int not_happened = 0;
    int detected = 0;
    int not_detected = 0;
    int no_fault_input = 0;
    float rate;
    int input_flag;
// fault parameters
    int size, fault_t;
    unsigned long start_bit, bit_position;

    const char *N_str = "27689145254652277648317052786652408727034013347861916344618412945922935785293212341925722097485611308582199278934136536546616563415785040542393683822008661609536698729947150129496161404251483519198125653552204388548857756067887228211945312609255862316276752825454884791200821838218709318743476721977586345617129633712243030109382196478094451250906032929224993757152937092876480451167392036944819577026396304020102937573692814502152677281475985382616770198004999884387286487884261774923589846450401358551617346731179249424778781948846488697047598152011076142101403216368658683771978570984437306790235378876558293822233";
    int word_size = 64;

    mpz_ui_pow_ui(b, 2, word_size); // b=2^word_size
    mpz_set_str(N, N_str, 10);

// precomputation
    mpz_invert(N_prime, N, b);
    mpz_sub(N_prime, b, N_prime);
    
    mpz_setbit(range_u, 2000);  //size of u in bits
    mpz_setbit(range_v, 2000);  //size of v in bits

    int iteration = 10000;

    int l = 2; // up to n

    int model_input; // Scenario 2
    
    // fault model: 1 => random bit flipping
    // fault model: 2 => random bit Stuck at 1
    // fault model: 3 => burst bit flipping
    // fault model: 4 => burst bit Stuck at 1
    // fault model: 5 => total random

    model_input = 4;

    int n1 = 0; // number of fault injection to u1
    int n2 = 0; // number of fault injection to v1
    int n3 = 0; // number of fault injection to u2
    int n4 = 0; // number of fault injection to v2

// only for checking delete later
    int equality1 = 0;
    int equality2 = 0;
    int equality_t = 0;
    int t_detected = 0;

    int n;

// main simulation loop. running for "iteration" times

    for (int i = 0; i < iteration; i ++)
    {
        input_flag = 0;
        // creating random u and v with maximum size of range_u and range_v
        grand(range_u, u);
        grand(range_v, v);

        // the number of iteration in the multiprecision algorithm
        n = mpz_sizeinbase(N, 2)/word_size;
        mpz_pow_ui(R, b, n);

        // No encodings on Main computation
        mpz_set(u1, u);
        mpz_set(v1, v);

        // Encodings on Recomputation
        E_index_u = 1;
        E_index_v = 1;

        mpz_mul_2exp(v2, v, E_index_v); // v2 = v * (2^E_index_v)
        mpz_mul_2exp(u2, u, E_index_u); // u2 = u * (2^E_index_u)


///////////////////////////// Fault injections /////////////////////////////

/////// Model: Random

        // Type: Flipping
        if (model_input == 1)  
        {
            // u1
            mpz_set(u1f, u1);
            size = calculateBitSize(u1);
            mpz_init_set_ui(range_flip, size);

            for (int i1 = 0; i1 < n1; i1++)
            {
                grand(range_flip, z);
                mpz_ui_pow_ui(temp_mul, 2, mpz_get_ui(z));  
                mpz_xor(u1f, u1f, temp_mul);
            }

            // v1
            mpz_set(v1f, v1);
            size = calculateBitSize(v1);
            mpz_init_set_ui(range_flip, size);

            for (int i2 = 0; i2 < n2; i2++)
            {
                grand(range_flip, z);
                mpz_ui_pow_ui(temp_mul, 2, mpz_get_ui(z)); 
                mpz_xor(v1f, v1f, temp_mul);
            }

            // u2
            mpz_set(u2f, u2);
            size = calculateBitSize(u2);
            mpz_init_set_ui(range_flip, size);

            for (int i3 = 0; i3 < n3; i3++)
            {
                grand(range_flip, z);
                mpz_ui_pow_ui(temp_mul, 2, mpz_get_ui(z)); 
                mpz_xor(u2f, u2f, temp_mul);
            }

            // v2
            mpz_set(v2f, v2);
            size = calculateBitSize(v2);
            mpz_init_set_ui(range_flip, size);

            for (int i4 = 0; i4 < n4; i4++)
            {
                grand(range_flip, z);
                mpz_ui_pow_ui(temp_mul, 2, mpz_get_ui(z)); 
                mpz_xor(v2f, v2f, temp_mul);
            }

        }

        // Type: Stuck at 1
        if (model_input == 2)
        {
            // u1
            mpz_set(u1f, u1);
            size = calculateBitSize(u1);
            mpz_init_set_ui(range_flip, size);

            for (int i1 = 0; i1 < n1; i1++)
            {
                grand(range_flip, z);
                mpz_ui_pow_ui(temp_mul, 2, mpz_get_ui(z));  
                mpz_ior(u1f, u1f, temp_mul);
            }

            // v1
            mpz_set(v1f, v1);
            size = calculateBitSize(v1);
            mpz_init_set_ui(range_flip, size);

            for (int i2 = 0; i2 < n2; i2++)
            {
                grand(range_flip, z);
                mpz_ui_pow_ui(temp_mul, 2, mpz_get_ui(z)); 
                mpz_ior(v1f, v1f, temp_mul);
            }

            // u2
            mpz_set(u2f, u2);
            size = calculateBitSize(u2);
            mpz_init_set_ui(range_flip, size);

            for (int i3 = 0; i3 < n3; i3++)
            {
                grand(range_flip, z);
                mpz_ui_pow_ui(temp_mul, 2, mpz_get_ui(z)); 
                mpz_ior(u2f, u2f, temp_mul);
            }

            // v2
            mpz_set(v2f, v2);
            size = calculateBitSize(v2);
            mpz_init_set_ui(range_flip, size);

            for (int i4 = 0; i4 < n4; i4++)
            {
                grand(range_flip, z);
                mpz_ui_pow_ui(temp_mul, 2, mpz_get_ui(z)); 
                mpz_ior(v2f, v2f, temp_mul);
            }

        }

/////// Model: Burst

        // Type: Flipping
        else if (model_input == 3)
        {
            // u1
            mpz_set(u1f, u1);
            size = calculateBitSize(u1);
            mpz_init_set_ui(range_flip, size);
            grand(range_flip, z);
            start_bit = mpz_get_ui(z) % size;

            for (int i1 = 0; i1 < n1; i1++)
            {
                bit_position = (start_bit + i1) % size;
                mpz_init_set_ui(mask, 1);
                mpz_mul_2exp(mask, mask, bit_position);
                mpz_xor(u1f, u1f, mask);
                mpz_clear(mask);
            }

            // v1
            mpz_set(v1f, v1);
            size = calculateBitSize(v1);
            mpz_init_set_ui(range_flip, size);
            grand(range_flip, z);
            start_bit = mpz_get_ui(z) % size;

            for (int i2 = 0; i2 < n2; i2++)
            {
                bit_position = (start_bit + i2) % size;
                mpz_init_set_ui(mask, 1);
                mpz_mul_2exp(mask, mask, bit_position);
                mpz_xor(v1f, v1f, mask);
                mpz_clear(mask);
            }

            // u2
            mpz_set(u2f, u2);
            size = calculateBitSize(u2);
            mpz_init_set_ui(range_flip, size);
            grand(range_flip, z);
            start_bit = mpz_get_ui(z) % size;

            for (int i3 = 0; i3 < n3; i3++)
            {
                bit_position = (start_bit + i3) % size;
                mpz_init_set_ui(mask, 1);
                mpz_mul_2exp(mask, mask, bit_position);
                mpz_xor(u2f, u2f, mask);
                mpz_clear(mask);
            }

            // v2
            mpz_set(v2f, v2);
            size = calculateBitSize(v2);
            mpz_init_set_ui(range_flip, size);
            grand(range_flip, z);
            start_bit = mpz_get_ui(z) % size;

            for (int i4 = 0; i4 < n4; i4++)
            {
                bit_position = (start_bit + i4) % size;
                mpz_init_set_ui(mask, 1);
                mpz_mul_2exp(mask, mask, bit_position);
                mpz_xor(v2f, v2f, mask);
                mpz_clear(mask);
            }
        }
        
        // Type: Stuck at 1
        else if (model_input == 4)
        {
            // u1
            mpz_set(u1f, u1);
            size = calculateBitSize(u1);
            mpz_init_set_ui(range_flip, size);
            grand(range_flip, z);
            start_bit = mpz_get_ui(z) % size;

            for (int i1 = 0; i1 < n1; i1++)
            {
                bit_position = (start_bit + i1) % size;
                mpz_init_set_ui(mask, 1);
                mpz_mul_2exp(mask, mask, bit_position);
                mpz_ior(u1f, u1f, mask);
                mpz_clear(mask);
            }

            // v1
            mpz_set(v1f, v1);
            size = calculateBitSize(v1);
            mpz_init_set_ui(range_flip, size);
            grand(range_flip, z);
            start_bit = mpz_get_ui(z) % size;

            for (int i2 = 0; i2 < n2; i2++)
            {
                bit_position = (start_bit + i2) % size;
                mpz_init_set_ui(mask, 1);
                mpz_mul_2exp(mask, mask, bit_position);
                mpz_ior(v1f, v1f, mask);
                mpz_clear(mask);
            }

            // u2
            mpz_set(u2f, u2);
            size = calculateBitSize(u2);
            mpz_init_set_ui(range_flip, size);
            grand(range_flip, z);
            start_bit = mpz_get_ui(z) % size;

            for (int i3 = 0; i3 < n3; i3++)
            {
                bit_position = (start_bit + i3) % size;
                mpz_init_set_ui(mask, 1);
                mpz_mul_2exp(mask, mask, bit_position);
                mpz_ior(u2f, u2f, mask);
                mpz_clear(mask);
            }

            // v2
            mpz_set(v2f, v2);
            size = calculateBitSize(v2);
            mpz_init_set_ui(range_flip, size);
            grand(range_flip, z);
            start_bit = mpz_get_ui(z) % size;

            for (int i4 = 0; i4 < n4; i4++)
            {
                bit_position = (start_bit + i4) % size;
                mpz_init_set_ui(mask, 1);
                mpz_mul_2exp(mask, mask, bit_position);
                mpz_ior(v2f, v2f, mask);
                mpz_clear(mask);
            }
        }

// TOTAL RANDOM
        else if (model_input == 5)
        {
            // u1
            size = calculateBitSize(u1);
            mpz_setbit(range_flip, size);
            grand(range_flip, u1f);

            // v1
            size = calculateBitSize(v1);
            mpz_setbit(range_flip, size);
            grand(range_flip, v1f);

            // u2
            size = calculateBitSize(u2);
            mpz_setbit(range_flip, size);
            grand(range_flip, u2f);

            // v2
            size = calculateBitSize(v2);
            mpz_setbit(range_flip, size);
            grand(range_flip, v2f);
        }


/************************************************************************************************************/
        if (mpz_cmp(u1, u1f) == 0 && mpz_cmp(v1, v1f) == 0 && mpz_cmp(u2, u2f) == 0 && mpz_cmp(v2, v2f) == 0)
        {
            no_fault_input ++; // might happen in stuck at 1 scenarios
            input_flag = 1;

        }
        

        montgomery_mult(result, partial_result1, weight1, u1f, v1f, N, n, word_size, b, N_prime, l, &fault_t);                  // Main computation
        montgomery_recomputation(partial_result2, weight2, u2f, v2f, N, n, word_size, b, N_prime, l, E_index_u, E_index_v);     // Recomputation


    //check the result:
        mpz_mul(result, result, R);
        mpz_mod(result, result, N);

        mpz_mul(answer, u, v);
        mpz_mod(answer, answer, N);

///////////// Comparison and detection /////////////    

        if (mpz_cmp(answer, result) == 0)
        {
            // no fault happened
            not_happened ++;
        }

        if (mpz_cmp(partial_result1, partial_result2) != 0 || mpz_cmp(weight1, weight2) != 0 || fault_t != n)
        {
            // fault happened and got detected
            detected ++;
        }

        if (mpz_cmp(answer, result) != 0 && mpz_cmp(partial_result1, partial_result2) == 0 && mpz_cmp(weight1, weight2) == 0 && fault_t == n)
        {
            // fault happened and not detected
            not_detected ++;
        }
        
    }
    
    rate = 100 - (100 * (not_detected) / iteration);

    printf("number of not detected: %d\n", not_detected);
    printf("number of no faults: %d\n", not_happened);
    printf("number of detected:     %d\n", detected);
    printf("fault rate is %.4f\n", rate);

    // Clear variables
    mpz_clears(weight1, weight2, u1, u2, v1, v2, u1f, u2f, v1f, v2f, range_flip, temp_mul, z, b, R, N, N_prime, u, v, answer, result, partial_result1, partial_result2, range_u, range_v, NULL);
    gmp_randclear(rand_state);

    return 0;
}

// gcc sim.c -o sim -lgmp