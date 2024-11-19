from __future__ import annotations

from typing import TYPE_CHECKING

import numpy as np
import pytest
import stim
from numpy.core.numeric import array_equal

if TYPE_CHECKING:
    from numpy.typing import NDArray
from mqt.qecc.cc_decoder.stim_interface.dem_to_matrices import (
    detector_error_model_to_check_matrices,
    dict_to_csc_matrix,
)


@pytest.fixture
def dem_matrix() -> NDArray[NDArray[int]]:
    return np.array([
        [1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    ])


@pytest.fixture
def hyperedges() -> dict[int, frozenset[int]]:
    return {
        0: frozenset({0, 1, 2}),
        1: frozenset({0, 1}),
        2: frozenset({0, 2}),
        3: frozenset({0, 3}),
        4: frozenset({0}),
        5: frozenset({1, 2}),
        6: frozenset({1, 4}),
        7: frozenset({1}),
        8: frozenset({2}),
        9: frozenset({2, 5}),
        10: frozenset({3, 4, 5}),
        11: frozenset({3, 4}),
        12: frozenset({3, 5}),
        13: frozenset({3, 6}),
        14: frozenset({3}),
        15: frozenset({4, 5}),
        16: frozenset({4, 7}),
        17: frozenset({4}),
        18: frozenset({5}),
        19: frozenset({8, 5}),
        20: frozenset({8, 6, 7}),
        21: frozenset({6, 7}),
        22: frozenset({8, 6}),
        23: frozenset({9, 6}),
        24: frozenset({6}),
        25: frozenset({8, 7}),
        26: frozenset({10, 7}),
        27: frozenset({7}),
        28: frozenset({8}),
        29: frozenset({8, 11}),
    }


@pytest.fixture
def hypergraph_shape() -> tuple[int, int]:
    return (12, 30)


@pytest.fixture
def priors() -> NDArray[np.float32]:
    return np.array([
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
        0.5,
    ])


@pytest.fixture
def hyperedge_to_edge_matrix() -> NDArray[NDArray[np.int32]]:
    return np.array([
        [
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
        ],
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            1,
        ],
    ])


@pytest.fixture
def edge_obsbl_matrix() -> NDArray[NDArray[np.int32]]:
    return np.array([[1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0]])


@pytest.fixture
def edge_check_matrix() -> NDArray[NDArray.np.int32]:
    return np.array([
        [1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    ])


@pytest.fixture
def obsble_matrix() -> NDArray[NDArray[np.int32]]:
    return np.array([[0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0]])


@pytest.fixture
def hamming_code() -> NDArray[bool]:
    return np.array([
        [True, True, False, True, True, False, False],
        [False, True, True, False, True, True, False],
        [False, False, False, True, True, True, True],
    ])


@pytest.fixture
def detector_error_model() -> stim.DetectorErrorModel:
    return stim.DetectorErrorModel("""
        error(0.5) D0 D1 D2
        error(0.5) D0 D1 L0
        error(0.5) D0 D2
        error(0.5) D0 D3
        error(0.5) D0 L0
        error(0.5) D1 D2
        error(0.5) D1 D4
        error(0.5) D1 L0
        error(0.5) D2
        error(0.5) D2 D5
        error(0.5) D3 D4 D5
        error(0.5) D3 D4 L0
        error(0.5) D3 D5
        error(0.5) D3 D6
        error(0.5) D3 L0
        error(0.5) D4 D5
        error(0.5) D4 D7
        error(0.5) D4 L0
        error(0.5) D5
        error(0.5) D5 D8
        error(0.5) D6 D7 D8
        error(0.5) D6 D7 L0
        error(0.5) D6 D8
        error(0.5) D6 D9
        error(0.5) D6 L0
        error(0.5) D7 D8
        error(0.5) D7 D10
        error(0.5) D7 L0
        error(0.5) D8
        error(0.5) D8 D11
        error(0.5) D0 D1 D2
        error(0.5) D0 D1 L0
        error(0.5) D0 D2
        error(0.5) D0 D3
        error(0.5) D0 L0
        error(0.5) D1 D2
        error(0.5) D1 D4
        error(0.5) D1 L0
        error(0.5) D2
        error(0.5) D2 D5
        error(0.5) D3 D4 D5
        error(0.5) D3 D4 L0
        error(0.5) D3 D5
        error(0.5) D3 D6
        error(0.5) D3 L0
        error(0.5) D4 D5
        error(0.5) D4 D7
        error(0.5) D4 L0
        error(0.5) D5
        error(0.5) D5 D8
        error(0.5) D6 D7 D8
        error(0.5) D6 D7 L0
        error(0.5) D6 D8
        error(0.5) D6 D9
        error(0.5) D6 L0
        error(0.5) D7 D8
        error(0.5) D7 D10
        error(0.5) D7 L0
        error(0.5) D8
        error(0.5) D8 D11
        error(0.5) D0 D1 D2
        error(0.5) D0 D1 L0
        error(0.5) D0 D2
        error(0.5) D0 D3
        error(0.5) D0 L0
        error(0.5) D1 D2
        error(0.5) D1 D4
        error(0.5) D1 L0
        error(0.5) D2
        error(0.5) D2 D5
        error(0.5) D3 D4 D5
        error(0.5) D3 D4 L0
        error(0.5) D3 D5
        error(0.5) D3 D6
        error(0.5) D3 L0
        error(0.5) D4 D5
        error(0.5) D4 D7
        error(0.5) D4 L0
        error(0.5) D5
        error(0.5) D5 D8
        error(0.5) D6 D7 D8
        error(0.5) D6 D7 L0
        error(0.5) D6 D8
        error(0.5) D6 D9
        error(0.5) D6 L0
        error(0.5) D7 D8
        error(0.5) D7 D10
        error(0.5) D7 L0
        error(0.5) D8
        error(0.5) D8 D11
        error(0.5) D0 D1 D2
        error(0.5) D0 D1 L0
        error(0.5) D0 D2
        error(0.5) D0 D3
        error(0.5) D0 L0
        error(0.5) D1 D2
        error(0.5) D1 D4
        error(0.5) D1 L0
        error(0.5) D2
        error(0.5) D2 D5
        error(0.5) D3 D4 D5
        error(0.5) D3 D4 L0
        error(0.5) D3 D5
        error(0.5) D3 D6
        error(0.5) D3 L0
        error(0.5) D4 D5
        error(0.5) D4 D7
        error(0.5) D4 L0
        error(0.5) D5
        error(0.5) D5 D8
        error(0.5) D6 D7 D8
        error(0.5) D6 D7 L0
        error(0.5) D6 D8
        error(0.5) D6 D9
        error(0.5) D6 L0
        error(0.5) D7 D8
        error(0.5) D7 D10
        error(0.5) D7 L0
        error(0.5) D8
        error(0.5) D8 D11
        error(0.5) D0 D1 D2
        error(0.5) D0 D1 L0
        error(0.5) D0 D2
        error(0.5) D0 D3
        error(0.5) D0 L0
        error(0.5) D1 D2
        error(0.5) D1 D4
        error(0.5) D1 L0
        error(0.5) D2
        error(0.5) D2 D5
        error(0.5) D3 D4 D5
        error(0.5) D3 D4 L0
        error(0.5) D3 D5
        error(0.5) D3 D6
        error(0.5) D3 L0
        error(0.5) D4 D5
        error(0.5) D4 D7
        error(0.5) D4 L0
        error(0.5) D5
        error(0.5) D5 D8
        error(0.5) D6 D7 D8
        error(0.5) D6 D7 L0
        error(0.5) D6 D8
        error(0.5) D6 D9
        error(0.5) D6 L0
        error(0.5) D7 D8
        error(0.5) D7 D10
        error(0.5) D7 L0
        error(0.5) D8
        error(0.5) D8 D11
        error(0.5) D0 D1 D2
        error(0.5) D0 D1 L0
        error(0.5) D0 D2
        error(0.5) D0 D3
        error(0.5) D0 L0
        error(0.5) D1 D2
        error(0.5) D1 D4
        error(0.5) D1 L0
        error(0.5) D2
        error(0.5) D2 D5
        error(0.5) D3 D4 D5
        error(0.5) D3 D4 L0
        error(0.5) D3 D5
        error(0.5) D3 D6
        error(0.5) D3 L0
        error(0.5) D4 D5
        error(0.5) D4 D7
        error(0.5) D4 L0
        error(0.5) D5
        error(0.5) D5 D8
        error(0.5) D6 D7 D8
        error(0.5) D6 D7 L0
        error(0.5) D6 D8
        error(0.5) D6 D9
        error(0.5) D6 L0
        error(0.5) D7 D8
        error(0.5) D7 D10
        error(0.5) D7 L0
        error(0.5) D8
        error(0.5) D8 D11
        error(0.5) D0 D1 D2
        error(0.5) D0 D1 L0
        error(0.5) D0 D2
        error(0.5) D0 D3
        error(0.5) D0 L0
        error(0.5) D1 D2
        error(0.5) D1 D4
        error(0.5) D1 L0
        error(0.5) D2
        error(0.5) D2 D5
        error(0.5) D3 D4 D5
        error(0.5) D3 D4 L0
        error(0.5) D3 D5
        error(0.5) D3 D6
        error(0.5) D3 L0
        error(0.5) D4 D5
        error(0.5) D4 D7
        error(0.5) D4 L0
        error(0.5) D5
        error(0.5) D5 D8
        error(0.5) D6 D7 D8
        error(0.5) D6 D7 L0
        error(0.5) D6 D8
        error(0.5) D6 D9
        error(0.5) D6 L0
        error(0.5) D7 D8
        error(0.5) D7 D10
        error(0.5) D7 L0
        error(0.5) D8
        error(0.5) D8 D11
        error(0.5) D0 D1 D2
        error(0.5) D0 D1 L0
        error(0.5) D0 D2
        error(0.5) D0 D3
        error(0.5) D0 L0
        error(0.5) D1 D2
        error(0.5) D1 D4
        error(0.5) D1 L0
        error(0.5) D2
        error(0.5) D2 D5
        error(0.5) D3 D4 D5
        error(0.5) D3 D4 L0
        error(0.5) D3 D5
        error(0.5) D3 D6
        error(0.5) D3 L0
        error(0.5) D4 D5
        error(0.5) D4 D7
        error(0.5) D4 L0
        error(0.5) D5
        error(0.5) D5 D8
        error(0.5) D6 D7 D8
        error(0.5) D6 D7 L0
        error(0.5) D6 D8
        error(0.5) D6 D9
        error(0.5) D6 L0
        error(0.5) D7 D8
        error(0.5) D7 D10
        error(0.5) D7 L0
        error(0.5) D8
        error(0.5) D8 D11
        error(0.5) D0 D1 D2
        error(0.5) D0 D1 L0
        error(0.5) D0 D2
        error(0.5) D0 D3
        error(0.5) D0 L0
        error(0.5) D1 D2
        error(0.5) D1 D4
        error(0.5) D1 L0
        error(0.5) D2
        error(0.5) D2 D5
        error(0.5) D3 D4 D5
        error(0.5) D3 D4 L0
        error(0.5) D3 D5
        error(0.5) D3 D6
        error(0.5) D3 L0
        error(0.5) D4 D5
        error(0.5) D4 D7
        error(0.5) D4 L0
        error(0.5) D5
        error(0.5) D5 D8
        error(0.5) D6 D7 D8
        error(0.5) D6 D7 L0
        error(0.5) D6 D8
        error(0.5) D6 D9
        error(0.5) D6 L0
        error(0.5) D7 D8
        error(0.5) D7 D10
        error(0.5) D7 L0
        error(0.5) D8
        error(0.5) D8 D11
        error(0.5) D0 D1 D2
        error(0.5) D0 D1 L0
        error(0.5) D0 D2
        error(0.5) D0 D3
        error(0.5) D0 L0
        error(0.5) D1 D2
        error(0.5) D1 D4
        error(0.5) D1 L0
        error(0.5) D2
        error(0.5) D2 D5
        error(0.5) D3 D4 D5
        error(0.5) D3 D4 L0
        error(0.5) D3 D5
        error(0.5) D3 D6
        error(0.5) D3 L0
        error(0.5) D4 D5
        error(0.5) D4 D7
        error(0.5) D4 L0
        error(0.5) D5
        error(0.5) D5 D8
        error(0.5) D6 D7 D8
        error(0.5) D6 D7 L0
        error(0.5) D6 D8
        error(0.5) D6 D9
        error(0.5) D6 L0
        error(0.5) D7 D8
        error(0.5) D7 D10
        error(0.5) D7 L0
        error(0.5) D8
        error(0.5) D8 D11""")


def test_dict_to_csc_matrix(
    hypergraph_shape, hyperedges: dict[int, frozenset[int]], dem_matrix: NDArray[NDArray[int]]
) -> None:
    result = dict_to_csc_matrix(hyperedges, hypergraph_shape).todense()
    assert array_equal(result, dem_matrix)


def test_detector_error_model_to_check_matrices(
    detector_error_model,
    priors,
    dem_matrix,
    obsble_matrix,
    edge_check_matrix,
    edge_obsbl_matrix,
    hyperedge_to_edge_matrix,
) -> None:
    result = detector_error_model_to_check_matrices(detector_error_model, True)
    assert array_equal(result.priors, priors)
    assert array_equal(result.check_matrix.todense(), dem_matrix)
    assert array_equal(result.observables_matrix.todense(), obsble_matrix)
    assert array_equal(result.edge_check_matrix.todense(), edge_check_matrix)
    assert array_equal(result.edge_observables_matrix.todense(), edge_obsbl_matrix)
    assert array_equal(result.hyperedge_to_edge_matrix.todense(), hyperedge_to_edge_matrix)
