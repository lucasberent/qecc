# Encoding Qubits: 2
OPENQASM 2.0;
include "qelib1.inc";
qreg q[7];
h q[4];
h q[5];
h q[6];
cx q[5],q[1];
cx q[1],q[2];
cx q[4],q[0];
cx q[6],q[4];
cx q[5],q[3];
cx q[2],q[0];
cx q[6],q[3];
cx q[4],q[5];
cx q[0],q[1];
