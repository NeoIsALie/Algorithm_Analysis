from scipy import optimize
from numpy.random import normal
from math import isclose
from psopy import minimize

def generate_delta(mu=0, sigma=1, num=1000):
    delta = normal(mu, sigma, num)
    return delta

def generator_function(x):
    f = 1 / (x ** 2 - 3 * x + 2)
    return f

def y_generator(f_x, delta_k):
    y = None
    if f_x < -100. and not isclose(f_x, -100, rel_tol=1e-5):
        y = -100. + delta_k
    elif abs(f_x) <= 100 and not isclose(f_x, 100., rel_tol=1e-5) and not isclose(f_x, -100., rel_tol=1e-5):
        y = f_x + delta_k
    elif f_x > 100. and not isclose(f_x, 100., rel_tol=1e-5):
        y = 100 + delta_k

    return y


def generate_points(num=1000):
    delta = generate_delta()
    data = [[3 * k / 1000., y_generator(generator_function(3 * k / 1000.), delta[k])] for k in range(num)]
    return data

def rational_approximation(x, a, b, c, d):
    f = (a * x + b) / (x ** 2 + c * x + d)
    return f
#
def least_square_func(approx, a, b, c, d, data):
    d = sum(pow(approx(a, b, c, d, x) - y, 2) for x, y  in data)
    return d

if __name__ == "__main__":
    generated_data = generate_points()
    print(generated_data)

    # res = optimize.minimize(least_square_func, x0=[0.1, 0.1, 0.1, 0.1],
    #                         args=(rational_approximation, generated_data),
    #                         method='nelder-mead')
    # res_lm = optimize.least_squares(least_square_func, [0.1, 0.1, 0.1, 0.1, 0.1],
    #                                 args=(rational_approximation, generated_data))
    # # res_anneal = optimize.dual_annealing()
    # res_de = optimize.differential_evolution(least_square_func, x0 = [0.1, 0.1, 0.1, 0.1, 0.1],
    #                                          args=(rational_approximation, generated_data),
    #                                          maxiter=1000)
    #
    # res_pso = minimize(least_square_func, args=(rational_approximation, generated_data))
    # print(res.x, res_lm.x)
