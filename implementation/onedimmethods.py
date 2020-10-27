from math import sin, sqrt
from scipy.optimize import *


def fun1(x):
    """

    :param x: x in D(x)
    :return: f(x) in E(x)
    """
    return x ** 3


def fun2(x):
    """

    :param x: x in D(x)
    :return: f(x) in E(x)
    """
    return abs(x - 0.2)


def fun3(x):
    """

    :param x: x in D(x)
    :return: f(x) in E(x)
    """
    return x * sin(1 / x)


def exhaustive_search(function, limits, precision=1e-3):
    """

    :param function: function to minimize
    :param limits: borders of the interval of minimization
    :param precision: accuracy of minimization
    :return: point of minimum
    """
    iters, f_calls = 0, 0
    x_min, f_min, x_curr = None, None, limits[0]

    while x_curr <= limits[1]:
        f_curr = function(x_curr)
        if x_min is None or f_curr < f_min:
            x_min, f_min = x_curr, f_curr
        f_calls += 1
        iters += 1
        x_curr += precision

    print("Number of iterations: ", iters,
          "Number of function calls: ", f_calls)

    return x_min, f_min


def dichotomy(function, limits, precision=1e-3):
    """

    :param function: function to minimize
    :param limits: borders of the interval of minimization
    :param precision: accuracy of minimization
    :return: point of minimum
    """
    delta = precision / 4
    a, b = limits
    iters, f_calls = 0, 0
    x_1, x_2 = (a + b - delta) / 2, (a + b + delta) / 2

    while abs(b - a) >= precision:
        f_1, f_2 = function(x_1), function(x_2)
        if f_1 <= f_2:
            b = x_2
        else:
            a = x_1
        x_1, x_2 = (a + b - delta) / 2, (a + b + delta) / 2
        iters, f_calls = iters + 1, f_calls + 2

    x_min, f_min = (a + b) / 2, function((a + b) / 2)

    print("Number of iterations: ", iters,
          "Number of function calls: ", f_calls)

    return x_min, f_min


def golden_section(function, limits, precision):
    """

    :param function: function to minimize
    :param limits: borders of the interval of minimization
    :param precision: accuracy of minimization
    :return: point of minimum
    """
    a, b = limits
    phi = (sqrt(5) + 1) / 2
    iters, f_calls = 0, 2

    x_1, x_2 = b - (b - a) / phi, a + (b - a) / phi
    f_1, f_2 = function(x_1), function(x_2)
    while abs(b - a) > precision:
        if f_1 < f_2:
            b = x_2
            x_2 = x_1
            x_1 = b - (b - a) / phi
        else:
            a = x_1
            x_1 = x_2
            x_2 = a + (b - a) / phi
        iters, f_calls = iters + 1, f_calls + 1

    print("Number of iterations: ", iters,
          "Number of function calls: ", f_calls)

    x_min, f_min = (a + b) / 2, function((a + b) / 2)

    return x_min, f_min


if __name__ == "__main__":
    x_min, f_min = exhaustive_search(fun1, [0, 1], precision=1e-3)
    print(x_min, f_min)
    x_min, f_min = dichotomy(fun1, [0, 1], precision=1e-3)
    print(x_min, f_min)
    x_min, f_min = golden_section(fun1, [0,1], precision=1e-3)
    print(x_min, f_min)
    x_min, f_min = exhaustive_search(fun2, [0, 1], precision=1e-3)
    print(x_min, f_min)
    x_min, f_min = dichotomy(fun2, [0, 1], precision=1e-3)
    print(x_min, f_min)
    x_min, f_min = golden_section(fun2, [0,1], precision=1e-3)
    print(x_min, f_min)
    x_min, f_min = exhaustive_search(fun3, [0.1, 1], precision=1e-3)
    print(x_min, f_min)
    x_min, f_min = dichotomy(fun3, [0.1, 1], precision=1e-3)
    print(x_min, f_min)
    x_min, f_min = golden_section(fun3, [0.1,1], precision=1e-3)
    print(x_min, f_min)
