from random import random, normalvariate, seed
from matplotlib import pyplot as plt
from scipy import optimize
from numpy import array


def get_data(n=101):
    alpha, beta = random(), random()
    data = [(x, alpha * x + beta + delta) for x, delta in [(k / 100., normalvariate(0, 1)) for k in range(n)]]

    return data


def linear(x, a, b):
    return a * x + b


def rational(x, a, b):
    return a / (1 + b * x)


def least_squares(coefs, approximant, data):
    d = sum(pow(approximant(x, *coefs) - y, 2) for x, y in data)
    return d


def exhaustive_search(fun, approximant, data, limits, precision=1e-3):
    x = limits[0][0]
    x_min, y_min, f_min = None, None, None
    iters, f_calls = 0, 0
    while x <= limits[1][0]:
        y = limits[0][1]
        while y <= limits[1][1]:
            f_cur = fun((x, y), approximant, data)
            if f_min is None or f_cur < f_min:
                x_min, y_min, f_min = x, y, f_cur
            y += precision
            iters, f_calls = iters + 1, f_calls + 1
        x += precision

    print("Number of iterations: ", iters,
          "Number of function calls: ", f_calls)

    return (x_min, y_min), f_min


def gauss(fun, approximant, data, limits, precision=1e-3):
    vec_par, f_min, f_min_prev = list(limits[0]), None, None
    k, cnt_par = 0, len(vec_par)
    iters, f_calls = 0, 0
    while True:
        vec_cur = vec_par.copy()
        vec_cur[k] = limits[0][k]
        while vec_cur[k] <= limits[1][k]:
            f_cur = fun(vec_cur, approximant, data)
            if f_min is None or f_cur < f_min:
                vec_par[k], f_min = vec_cur[k], f_cur
            vec_cur[k] += precision * 2
            iters, f_calls = iters + 1, f_calls + 1
        k = (k + 1) % cnt_par
        if f_min_prev is None or abs(f_min - f_min_prev) > precision:
            f_min_prev = f_min
        else:
            break

    print("Number of iterations: ", iters,
          "Number of function calls: ", f_calls)

    return (vec_par[0], vec_par[1]), f_min


if __name__ == "__main__":
    seed()

    data = get_data()

    x, y = list(map(lambda k: k[0], data)), list(map(lambda k: k[1], data))
    limits = ((min(x), min(y)), (max(x), max(y)))

    fig, ax = plt.subplots(2)
    titles = ["Linear approximation", "Rational approximation"]
    funcs = [linear, rational]
    for i in range(2):
        ax[i].set_title(titles[i])
        ax[i].grid(1)
        ax[i].scatter(x, y, color="blue", marker=".", label="data")

        print(titles[i])
        coefficients_1, f_1 = exhaustive_search(least_squares, funcs[i], data, limits)
        print("Exhaustive search: ", coefficients_1[0], coefficients_1[1], f_1)
        ax[i].plot(x, [funcs[i](j, coefficients_1[0], coefficients_1[1]) for j in x],
                   color="c", label="ExS")
        coefficients_2, f_2 = gauss(least_squares, funcs[i], data, limits)
        print("Gauss_method", coefficients_2[0], coefficients_2[1], f_2)
        ax[i].plot(x, [funcs[i](j, coefficients_2[0], coefficients_2[1]) for j in x], color="m", label="Gauss")

        res = optimize.minimize(least_squares, x0=array([0.1, 0.1]),
                                args=(funcs[i], data),
                                method='nelder-mead')
        ax[i].plot(x, [funcs[i](j, res.x[0], res.x[1]) for j in x], color="r", label="NM")
        print(res.x, res.nit, res.nfev)

        ax[i].legend()
    plt.show()
