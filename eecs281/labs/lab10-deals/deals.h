#ifndef DEALS_H
#define DEALS_H

// EECS 281 LAB 10: DYNAMIC PROGRAMMING
// Identifier: D7E20F91029D0CB08715A2C54A782E0E8DF829BF

// Your favorite restaurant has a customer loyalty program. It works like this: whenever you buy a
// meal, you can add one holepunch to your punchcard. Once you have 5 punches, you can turn in the
// card for a free meal (and a new, unmarked punchcard).

// For example, if your meals cost [3, 3, 3, 3, 3, 3, 3, 120] then you should earn hole punches
// from the first 5 meals ($15), pay normally for the next two, and then turn in the punchcard
// so that the $120 meal is free! The total cost would be $21 (or with coupons [see below] $19).

// However, you ALSO have a lot of coupons to this restaurant. In fact, you have enough coupons
// that you can apply one to any and every meal! If you apply a coupon, you get a 25% discount
// on that meal. HOWEVER, you don't get to add a holepunch to your card if you use a coupon!

// For example, if your meals cost [2, 2, 2, 2, 1000, 100], then if you use the first 5 meals to
// earn holepunches, you'll need to spend $1008. Then you'll get $100 free. It would be much better
// to just apply the 25% discount to each item, so that you pay a total of $829.

// There are, however, many cases where it makes sense to use a mixture of punchcard discounts and
// discounting coupons. This is where your program comes in!

// You will be given a list of meal prices. For the given meal prices, find the LEAST POSSIBLE
// cost needed to pay for all the meals using the holepunch loyalty program and coupons.

// Notes/Clarifications/Hints:
// * use the "discounted" function below to compute discount
//    (so that you round the same way as the instructor solution).
// * you should always EITHER use a coupon, or apply the punchcard
// * you have an unlimited number of coupons
// * your program should be able to run in linear time.
// * greedy solutions will not work
// * use dynamic programming

#include <iostream>
#include <vector>
#include <functional>
#include <limits>

// TODO: import whatever you want

// This is a type synonym.
using cost = long long;

// How much you pay for a discounted (25% off) meal.
cost discounted(cost full_price) {
    return full_price * 3 / 4;
}

cost best_price(const std::vector<cost>& prices) {
    size_t num_meals = prices.size();

    // Create a 2D DP table init w inifinity.
    std::vector<std::vector<cost>> dp(6, std::vector<cost>(num_meals, std::numeric_limits<cost>::max() / 4));

    // Init first meal dp[0][0] with discount
    dp[0][0] = discounted(prices[0]);

    // [1][0] is first meal at full price and starts the punch card
    dp[1][0] = prices[0];
    
    // Loop to handle the cost calculations for the first 5 meals
    for (size_t i = 1; i < 5; i++) {
        // apply discounts for the first five meal
        dp[0][i] = dp[0][i - 1] + discounted(prices[i]);

        // Accumulate full price meals to get a free meal
        if (i < num_meals) dp[i + 1][i] = dp[i][i - 1] + prices[i];
    }


    for (size_t col = 1; col < 5; col++) {
        for (size_t row = 1; row <= col; row++) {
            // Calculate the minimum cost
            dp[row][col] = std::min(dp[row - 1][col - 1] + prices[col], dp[row][col - 1] + discounted(prices[col]));
        }
    }

    // beyond the fifth meal where a free meal can be earned.
    for (size_t col = 5; col < num_meals; col++) {
        for (size_t row = 0; row < 6; row++) {
            // Handle the zero row separately
            if (row == 0) dp[row][col] = std::min(dp[row][col - 1] + discounted(prices[col]), dp[5][col - 1]);

            // Calculate cost by deciding whether to continue accumulating or apply discount
            else dp[row][col] = std::min(dp[row - 1][col - 1] + prices[col], dp[row][col - 1] + discounted(prices[col]));
        }
    }
    // Determine the minimum cost from the last meal
    cost min_cost = dp[0][num_meals - 1];
    for (size_t row = 1; row < 6; row++) {
        min_cost = std::min(min_cost, dp[row][num_meals - 1]);
    }
    return min_cost; // Return the computed minimum cost from the DP table.
}

#endif
