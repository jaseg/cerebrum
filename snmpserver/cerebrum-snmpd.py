#!/usr/bin/env python3
from pysnmp.entity import engine, config
from pysnmp.entity.rfc3413 import cmdrsp, context
from pysnmp.carrier.asynsock.dgram import udp

snmpEngine = engine.SnmpEngine()
config.addSocketTransport( snmpEngine, udp.domainName, udp.UdpTransport().openServerMode(('127.0.0.1', 161)) )
config.addV1System( snmpEngine, 'c-base', 'public' )
config.addV3User( snmpEngine, 'usr-none-none' )
config.addContest( snmpEngine, '' )
config.addVacmUser( snmpEngine, 1, 'c-base', 'noAuthNoPriv', (1,3,6), (1,3,6) )
config.addVacmUser( snmpEngine, 2, 'c-base', 'noAuthNoPriv', (1,3,6), (1,3,6) )
config.addVacmUser( snmpEngine, 3, 'usr-none-none', 'noAuthNoPriv', (1,3,6), (1,3,6) )

snmpContext = context.snmpContext( snmpEngine )
cmdrsp.GetCommandResponder( snmpEngine, snmpContext )
cmdrsp.SetCommandResponder( snmpEngine, snmpContext )
cmdrsp.NextCommandResponder( snmpEngine, snmpContext )
cmdrsp.BulkCommandResponder( snmpEngine, snmpContext )

snmpEngine.transportDispatcher.jobStarted(1)
try:
    snmpEngine.transportDispatcher.runDispatcher()
except:
    snmpEngine.transportDispatcher.closeDispatcher()
    raise
