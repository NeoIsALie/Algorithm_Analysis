from random import random, normalvariate, seed

from matplotlib import pyplot as plt
from matplotlib.font_manager import FontProperties

from numpy import finfo, array
from scipy.optimize import approx_fprime, minimize, least_squares


def linear(x, a, b):
    return a * x + b


def rational(x, a, b):
    return a / (1 + b * x)


def f_least_squares(coefs, function, data):
    d = sum(pow(function(x, *coefs) - y, 2) for x, y in data)
    return d


def get_data(n=101):
    alpha, beta = random(), random()
    data = [(x, alpha * x + beta + delta) for x, delta in [(k / 100., normalvariate(0, 1)) for k in range(n)]]

    return data


def gradient_descent(function, x0, args, m=None, alpha=1e-3,
                     precision=finfo(float).eps, iterations=10000):
    vec_x = list(x0)
    iteration = 1
    if not m:
        m = len(vec_x)

    while iteration <= iterations:
        function_cur = function(vec_x, *args)
        gradient = approx_fprime(vec_x, function, alpha, *args)

        for i in range(m):
            vec_x[i] += -alpha * gradient[i]

        if abs(function_cur) <= precision:
            break

        iteration += 1

    return vec_x, iteration

if __name__ == '__main__':
    seed()
    eps = 1e-3
    data = get_data()

    x, y = list(map(lambda k: k[0], data)), list(map(lambda k: k[1], data))
    limits = ((min(x), min(y)), (max(x), max(y)))

    fig, ax = plt.subplots(2)
    fontP = FontProperties()
    fontP.set_size('x-small')
    titles = ["Linear approximation", "Rational approximation"]
    funcs = [linear, rational]
    for i in range(2):
        ax[i].set_title(titles[i])
        ax[i].grid(1)
        ax[i].scatter(x, y, color="blue", marker=".", label="data")

        print(titles[i])
        print("Results : point, iterations, f_calls, g_calls(j_calls)")
        co_1, count = gradient_descent(f_least_squares, [0.1, 0.1], (funcs[i], data), precision=eps, iterations=200)
        print("Gradient Descent: ", co_1, count, count, count)
        ax[i].plot(x, [funcs[i](j, co_1[0], co_1[1]) for j in x], color="c", label="G")

        co_2 = minimize(f_least_squares, array([0.3, 0.9]), (funcs[i], data), method="CG", tol=eps)
        print("Conjugate Gradient Descent: ", co_2.x, co_2.nit, co_2.nfev, co_2.njev)
        ax[i].plot(x, [funcs[i](j, co_2.x[0], co_2.x[1]) for j in x], color="m", label="CG")

        co_3 = minimize(f_least_squares, array([0.8, -1.96]), (linear, data), 'Newton-CG',
                        jac=lambda xk, *args: approx_fprime(xk, f_least_squares, eps, *args), tol=eps)
        print("Newton: ", co_2.x, co_2.nit, co_2.nfev, co_2.njev)
        ax[i].plot(x, [funcs[i](j, co_3.x[0], co_3.x[1]) for j in x], color="r", label="N")

        co_4 = least_squares(f_least_squares, array([0.6, 0.2]), args=(funcs[i], data))
        print("Levenberg-Marquardt: ", co_2.x, co_2.nit, co_2.nfev, co_2.njev)
        ax[i].plot(x, [funcs[i](j, co_4.x[0], co_4.x[1]) for j in x], color="y", label="LM")


        ax[i].legend(loc='upper right', bbox_to_anchor=(1, 1), prop=fontP)
    plt.show()
