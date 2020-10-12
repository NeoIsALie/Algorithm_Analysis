from scipy import optimize
from numpy.random import normal
from math import isclose
from matplotlib import pyplot as plt
from numpy import array

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
def least_square_func(coefs, approx, data):
    d = sum(pow(approx(x, *coefs) - y, 2) for x, y  in data)
    return d

if __name__ == "__main__":
    generated_data = generate_points()

    x, y = list(map(lambda k: k[0], generated_data)), list(map(lambda k: k[1], generated_data))

    res = optimize.minimize(least_square_func, x0=array([0.1, 0.1, 0.1, 0.1]),
                            args=(rational_approximation, generated_data),
                            method='nelder-mead')
    res_lm = optimize.least_squares(least_square_func, [0.1, 0.1, 0.1, 0.1],
                                    args=(rational_approximation, generated_data), ftol=1e-3)
    res_anneal = optimize.dual_annealing(least_square_func, [(-5, 5), (-5, 5), (-5, 5), (-5, 5)],
                                         args=(rational_approximation, generated_data),
                                         maxiter=1000)
    res_de = optimize.differential_evolution(least_square_func, [(-5, 5), (-5, 5), (-5, 5), (-5, 5)],
                                             args=(rational_approximation, generated_data),
                                             maxiter=1000, tol=1e-3)

    print(res.x, res.nit, res.nfev)
    print(res_lm.x, res_lm.nfev)
    print(res_anneal.x, res_anneal.nit, res_anneal.nfev)
    print(res_de.x, res_de.nit, res_de.nfev)


    plt.plot(x, y, color="blue", marker=".", label="signal")
    plt.plot(x, [rational_approximation(j, res.x[0], res.x[1], res.x[2], res.x[3]) for j in x], color="green", label="Nelder-Mead")
    plt.plot(x, [rational_approximation(j, res_lm.x[0], res_lm.x[1], res_lm.x[2], res_lm.x[3]) for j in x], color="red", label="Levenberg-Marquardt")
    plt.plot(x, [rational_approximation(j, res_anneal.x[0], res_anneal.x[1], res_anneal.x[2], res_anneal.x[3]) for j in x], color="green", label="Simulated annealing")
    plt.plot(x, [rational_approximation(j, res_de.x[0], res_de.x[1], res_de.x[2], res_de.x[3]) for j in x], color="yellow", label="Differential evolution")

    plt.title('Rational approximation')
    plt.grid(1)
    plt.legend()
    plt.figure(figsize=(20,30))
    plt.show()
