<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="9.3.0">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="88" name="SimResults" color="9" fill="1" visible="yes" active="yes"/>
<layer number="89" name="SimProbes" color="9" fill="1" visible="yes" active="yes"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="myparts">
<description>&lt;b&gt;https://componentsearchengine.com&lt;/b&gt;&lt;p&gt;
&lt;author&gt;Created by SamacSys&lt;/author&gt;</description>
<packages>
<package name="SHT3X-DIS">
<smd name="1" x="-1.23825" y="0.75" dx="0.8255" dy="0.3048" layer="1"/>
<smd name="2" x="-1.23825" y="0.25" dx="0.8255" dy="0.3048" layer="1"/>
<smd name="3" x="-1.23825" y="-0.25" dx="0.8255" dy="0.3048" layer="1"/>
<smd name="4" x="-1.23825" y="-0.75" dx="0.8255" dy="0.3048" layer="1"/>
<smd name="5" x="1.23825" y="-0.75" dx="0.8255" dy="0.3048" layer="1"/>
<smd name="6" x="1.23825" y="-0.25" dx="0.8255" dy="0.3048" layer="1"/>
<smd name="7" x="1.23825" y="0.25" dx="0.8509" dy="0.3048" layer="1"/>
<smd name="8" x="1.23825" y="0.75" dx="0.8255" dy="0.3048" layer="1"/>
<smd name="9" x="0" y="0" dx="1.651" dy="0.889" layer="1" rot="R90"/>
<text x="-2.361" y="-0.3185" size="1.016" layer="25">&gt;NAME</text>
<text x="-2.361" y="-0.352" size="1.016" layer="27">&gt;VALUE</text>
<wire x1="-1.27" y1="1.27" x2="1.27" y2="1.27" width="0.0762" layer="51"/>
<wire x1="1.27" y1="1.27" x2="1.27" y2="-1.27" width="0.0762" layer="51"/>
<wire x1="1.27" y1="-1.27" x2="-1.27" y2="-1.27" width="0.0762" layer="51"/>
<wire x1="-1.27" y1="-1.27" x2="-1.27" y2="1.27" width="0.0762" layer="51"/>
<wire x1="-1.8415" y1="1.524" x2="1.8415" y2="1.524" width="0.127" layer="21"/>
<wire x1="1.8415" y1="1.524" x2="1.8415" y2="-1.524" width="0.127" layer="21"/>
<wire x1="1.8415" y1="-1.524" x2="-1.8415" y2="-1.524" width="0.127" layer="21"/>
<wire x1="-1.8415" y1="-1.524" x2="-1.8415" y2="1.524" width="0.127" layer="21"/>
<circle x="-1.5875" y="1.27" radius="0.1419875" width="0.127" layer="21"/>
</package>
<package name="HEADER-6P-DIP-2_54MM">
<description>&lt;b&gt;825433-5&lt;/b&gt;&lt;br&gt;
</description>
<pad name="1" x="0" y="0" drill="1.1" diameter="1.65"/>
<pad name="2" x="2.54" y="0" drill="1.1" diameter="1.65"/>
<pad name="3" x="5.08" y="0" drill="1.1" diameter="1.65"/>
<pad name="4" x="7.62" y="0" drill="1.1" diameter="1.65"/>
<pad name="5" x="10.16" y="0" drill="1.1" diameter="1.65"/>
<text x="5.08" y="0.889" size="1.016" layer="25" align="center">&gt;NAME</text>
<pad name="6" x="12.7" y="0" drill="1.1" diameter="1.65"/>
<wire x1="-1.27" y1="1.27" x2="13.97" y2="1.27" width="0.0762" layer="51"/>
<wire x1="13.97" y1="1.27" x2="13.97" y2="-1.27" width="0.0762" layer="51"/>
<wire x1="13.97" y1="-1.27" x2="-1.27" y2="-1.27" width="0.0762" layer="51"/>
<wire x1="-1.27" y1="-1.27" x2="-1.27" y2="1.27" width="0.0762" layer="51"/>
<wire x1="-1.27" y1="1.27" x2="13.97" y2="1.27" width="0.127" layer="21"/>
<wire x1="13.97" y1="1.27" x2="13.97" y2="-1.27" width="0.127" layer="21"/>
<wire x1="13.97" y1="-1.27" x2="-1.27" y2="-1.27" width="0.127" layer="21"/>
<wire x1="-1.27" y1="-1.27" x2="-1.27" y2="1.27" width="0.127" layer="21"/>
<text x="2.032" y="0.381" size="1.016" layer="27">&gt;VALUE</text>
</package>
<package name="0603">
<description>&lt;b&gt;ERA3A&lt;/b&gt;&lt;br&gt;
</description>
<smd name="1" x="-0.762" y="0" dx="0.889" dy="0.762" layer="1" rot="R90"/>
<smd name="2" x="0.762" y="0" dx="0.889" dy="0.762" layer="1" rot="R90"/>
<text x="0" y="0.254" size="1.016" layer="25" align="center">&gt;NAME</text>
<wire x1="-0.8" y1="0.4" x2="0.8" y2="0.4" width="0.0762" layer="51"/>
<wire x1="0.8" y1="0.4" x2="0.8" y2="-0.4" width="0.0762" layer="51"/>
<wire x1="0.8" y1="-0.4" x2="-0.8" y2="-0.4" width="0.0762" layer="51"/>
<wire x1="-0.8" y1="-0.4" x2="-0.8" y2="0.4" width="0.0762" layer="51"/>
<wire x1="-1.3335" y1="0.635" x2="1.3335" y2="0.635" width="0.127" layer="21"/>
<wire x1="1.3335" y1="0.635" x2="1.3335" y2="-0.635" width="0.127" layer="21"/>
<wire x1="1.3335" y1="-0.635" x2="-1.3335" y2="-0.635" width="0.127" layer="21"/>
<wire x1="-1.3335" y1="-0.635" x2="-1.3335" y2="0.635" width="0.127" layer="21"/>
<text x="-3.429" y="-0.254" size="1.016" layer="27">&gt;VALUE</text>
</package>
</packages>
<symbols>
<symbol name="SHT3X-DIS">
<wire x1="5.08" y1="2.54" x2="22.86" y2="2.54" width="0.254" layer="94"/>
<wire x1="22.86" y1="-10.16" x2="22.86" y2="2.54" width="0.254" layer="94"/>
<wire x1="22.86" y1="-10.16" x2="5.08" y2="-10.16" width="0.254" layer="94"/>
<wire x1="5.08" y1="2.54" x2="5.08" y2="-10.16" width="0.254" layer="94"/>
<text x="5.334" y="3.556" size="1.778" layer="95" align="center-left">&gt;NAME</text>
<text x="5.334" y="5.334" size="1.778" layer="96" align="center-left">&gt;VALUE</text>
<pin name="SDA" x="0" y="0" length="middle"/>
<pin name="ADDR" x="0" y="-2.54" length="middle"/>
<pin name="ALERT" x="0" y="-5.08" length="middle"/>
<pin name="SCL" x="0" y="-7.62" length="middle"/>
<pin name="VDD" x="27.94" y="-7.62" length="middle" direction="pwr" rot="R180"/>
<pin name="RES" x="27.94" y="-5.08" length="middle" rot="R180"/>
<pin name="R" x="27.94" y="-2.54" length="middle" rot="R180"/>
<pin name="VSS" x="27.94" y="0" length="middle" rot="R180"/>
<pin name="EP" x="15.24" y="-15.24" length="middle" direction="pwr" rot="R90"/>
</symbol>
<symbol name="GND">
<wire x1="-1.905" y1="0" x2="1.905" y2="0" width="0.254" layer="94"/>
<text x="-2.54" y="-3.048" size="1.778" layer="96">&gt;VALUE</text>
<pin name="GND" x="0" y="2.54" visible="off" length="short" direction="sup" rot="R270"/>
<wire x1="-1.27" y1="-0.508" x2="1.27" y2="-0.508" width="0.254" layer="94"/>
<wire x1="-0.508" y1="-1.016" x2="0.508" y2="-1.016" width="0.254" layer="94"/>
</symbol>
<symbol name="VCC">
<circle x="0" y="0.762" radius="0.762" width="0.254" layer="94"/>
<text x="-1.905" y="1.905" size="1.778" layer="96">&gt;VALUE</text>
<pin name="VCC" x="0" y="-2.54" visible="off" length="short" direction="sup" rot="R90"/>
</symbol>
<symbol name="CONNECTOR-6P">
<wire x1="5.08" y1="2.54" x2="12.7" y2="2.54" width="0.254" layer="94"/>
<wire x1="12.7" y1="-15.24" x2="12.7" y2="2.54" width="0.254" layer="94"/>
<wire x1="12.7" y1="-15.24" x2="5.08" y2="-15.24" width="0.254" layer="94"/>
<wire x1="5.08" y1="2.54" x2="5.08" y2="-15.24" width="0.254" layer="94"/>
<text x="5.08" y="3.556" size="1.778" layer="95" align="center-left">&gt;NAME</text>
<text x="5.08" y="5.334" size="1.778" layer="96" align="center-left">&gt;VALUE</text>
<pin name="1" x="0" y="0" length="middle"/>
<pin name="2" x="0" y="-2.54" length="middle"/>
<pin name="3" x="0" y="-5.08" length="middle"/>
<pin name="4" x="0" y="-7.62" length="middle"/>
<pin name="5" x="0" y="-10.16" length="middle"/>
<pin name="6" x="0" y="-12.7" length="middle"/>
</symbol>
<symbol name="CAPACITOR">
<wire x1="5.588" y1="2.032" x2="5.588" y2="-2.032" width="0.254" layer="94"/>
<wire x1="7.112" y1="2.032" x2="7.112" y2="-2.032" width="0.254" layer="94"/>
<wire x1="5.08" y1="0" x2="5.588" y2="0" width="0.254" layer="94"/>
<wire x1="7.112" y1="0" x2="7.62" y2="0" width="0.254" layer="94"/>
<text x="3.81" y="3.048" size="1.778" layer="95" align="center-left">&gt;NAME</text>
<text x="3.81" y="4.826" size="1.778" layer="96" align="center-left">&gt;VALUE</text>
<pin name="1" x="0" y="0" visible="off" length="middle" direction="pas"/>
<pin name="2" x="12.7" y="0" visible="off" length="middle" direction="pas" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="SHT3X-DIS" prefix="IC" uservalue="yes">
<gates>
<gate name="G$1" symbol="SHT3X-DIS" x="0" y="0"/>
</gates>
<devices>
<device name="" package="SHT3X-DIS">
<connects>
<connect gate="G$1" pin="ADDR" pad="2"/>
<connect gate="G$1" pin="ALERT" pad="3"/>
<connect gate="G$1" pin="EP" pad="9"/>
<connect gate="G$1" pin="R" pad="7"/>
<connect gate="G$1" pin="RES" pad="6"/>
<connect gate="G$1" pin="SCL" pad="4"/>
<connect gate="G$1" pin="SDA" pad="1"/>
<connect gate="G$1" pin="VDD" pad="5"/>
<connect gate="G$1" pin="VSS" pad="8"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="GND" prefix="GND" uservalue="yes">
<gates>
<gate name="1" symbol="GND" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="VCC" prefix="SUP" uservalue="yes">
<gates>
<gate name="G$1" symbol="VCC" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="HEADER-6P-DIP-2_54MM" prefix="CON" uservalue="yes">
<gates>
<gate name="G$1" symbol="CONNECTOR-6P" x="0" y="0"/>
</gates>
<devices>
<device name="" package="HEADER-6P-DIP-2_54MM">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
<connect gate="G$1" pin="3" pad="3"/>
<connect gate="G$1" pin="4" pad="4"/>
<connect gate="G$1" pin="5" pad="5"/>
<connect gate="G$1" pin="6" pad="6"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="CAPACITOR-0603" prefix="C" uservalue="yes">
<gates>
<gate name="G$1" symbol="CAPACITOR" x="0" y="0"/>
</gates>
<devices>
<device name="" package="0603">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="IC1" library="myparts" deviceset="SHT3X-DIS" device=""/>
<part name="GND2" library="myparts" deviceset="GND" device=""/>
<part name="GND3" library="myparts" deviceset="GND" device=""/>
<part name="GND4" library="myparts" deviceset="GND" device=""/>
<part name="C1" library="myparts" deviceset="CAPACITOR-0603" device=""/>
<part name="SUP2" library="myparts" deviceset="VCC" device=""/>
<part name="GND5" library="myparts" deviceset="GND" device=""/>
<part name="CON1" library="myparts" deviceset="HEADER-6P-DIP-2_54MM" device=""/>
<part name="SUP1" library="myparts" deviceset="VCC" device=""/>
<part name="GND1" library="myparts" deviceset="GND" device=""/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="IC1" gate="G$1" x="-12.7" y="2.54" smashed="yes">
<attribute name="NAME" x="-7.366" y="6.096" size="1.778" layer="95" align="center-left"/>
<attribute name="VALUE" x="-7.366" y="7.874" size="1.778" layer="96" align="center-left"/>
</instance>
<instance part="GND2" gate="1" x="17.78" y="2.54" smashed="yes" rot="R90">
<attribute name="VALUE" x="20.828" y="0" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="GND3" gate="1" x="-15.24" y="0" smashed="yes" rot="R270">
<attribute name="VALUE" x="-18.288" y="2.54" size="1.778" layer="96" rot="R270"/>
</instance>
<instance part="GND4" gate="1" x="15.24" y="-20.32" smashed="yes">
<attribute name="VALUE" x="12.7" y="-23.368" size="1.778" layer="96"/>
</instance>
<instance part="C1" gate="G$1" x="15.24" y="-17.78" smashed="yes" rot="R90">
<attribute name="NAME" x="11.938" y="-12.954" size="1.778" layer="95" rot="R90" align="center-left"/>
<attribute name="VALUE" x="10.414" y="-13.97" size="1.778" layer="96" rot="R90" align="center-left"/>
</instance>
<instance part="SUP2" gate="G$1" x="17.78" y="-5.08" smashed="yes" rot="R270">
<attribute name="VALUE" x="19.685" y="-3.175" size="1.778" layer="96" rot="R270"/>
</instance>
<instance part="GND5" gate="1" x="2.54" y="-15.24" smashed="yes">
<attribute name="VALUE" x="0" y="-18.288" size="1.778" layer="96"/>
</instance>
<instance part="CON1" gate="G$1" x="2.54" y="-30.48" smashed="yes" rot="R270">
<attribute name="NAME" x="6.096" y="-35.56" size="1.778" layer="95" rot="R270" align="center-left"/>
<attribute name="VALUE" x="7.874" y="-35.56" size="1.778" layer="96" rot="R270" align="center-left"/>
</instance>
<instance part="SUP1" gate="G$1" x="2.54" y="-27.94" smashed="yes">
<attribute name="VALUE" x="0.635" y="-26.035" size="1.778" layer="96"/>
</instance>
<instance part="GND1" gate="1" x="0" y="-27.94" smashed="yes" rot="R180">
<attribute name="VALUE" x="2.54" y="-24.892" size="1.778" layer="96" rot="R180"/>
</instance>
</instances>
<busses>
</busses>
<nets>
<net name="GND" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="VSS"/>
<pinref part="GND2" gate="1" pin="GND"/>
<pinref part="IC1" gate="G$1" pin="R"/>
<wire x1="15.24" y1="0" x2="15.24" y2="2.54" width="0.1524" layer="91"/>
<junction x="15.24" y="2.54"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="ADDR"/>
<pinref part="GND3" gate="1" pin="GND"/>
</segment>
<segment>
<pinref part="C1" gate="G$1" pin="1"/>
<pinref part="GND4" gate="1" pin="GND"/>
</segment>
<segment>
<pinref part="IC1" gate="G$1" pin="EP"/>
<pinref part="GND5" gate="1" pin="GND"/>
</segment>
<segment>
<pinref part="CON1" gate="G$1" pin="2"/>
<pinref part="GND1" gate="1" pin="GND"/>
</segment>
</net>
<net name="VCC" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="VDD"/>
<pinref part="C1" gate="G$1" pin="2"/>
<pinref part="SUP2" gate="G$1" pin="VCC"/>
<junction x="15.24" y="-5.08"/>
</segment>
<segment>
<pinref part="CON1" gate="G$1" pin="1"/>
<pinref part="SUP1" gate="G$1" pin="VCC"/>
</segment>
</net>
<net name="N$1" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="RES"/>
<wire x1="20.32" y1="-2.54" x2="15.24" y2="-2.54" width="0.1524" layer="91"/>
<wire x1="20.32" y1="-2.54" x2="20.32" y2="-22.86" width="0.1524" layer="91"/>
<wire x1="20.32" y1="-22.86" x2="-10.16" y2="-22.86" width="0.1524" layer="91"/>
<wire x1="-10.16" y1="-22.86" x2="-10.16" y2="-30.48" width="0.1524" layer="91"/>
<pinref part="CON1" gate="G$1" pin="6"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="SDA"/>
<wire x1="-12.7" y1="2.54" x2="-17.78" y2="2.54" width="0.1524" layer="91"/>
<wire x1="-17.78" y1="2.54" x2="-17.78" y2="-15.24" width="0.1524" layer="91"/>
<wire x1="-17.78" y1="-15.24" x2="-5.08" y2="-15.24" width="0.1524" layer="91"/>
<wire x1="-5.08" y1="-15.24" x2="-5.08" y2="-30.48" width="0.1524" layer="91"/>
<pinref part="CON1" gate="G$1" pin="4"/>
</segment>
</net>
<net name="N$3" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="SCL"/>
<wire x1="-12.7" y1="-5.08" x2="-12.7" y2="-10.16" width="0.1524" layer="91"/>
<wire x1="-12.7" y1="-10.16" x2="-2.54" y2="-10.16" width="0.1524" layer="91"/>
<wire x1="-2.54" y1="-10.16" x2="-2.54" y2="-30.48" width="0.1524" layer="91"/>
<pinref part="CON1" gate="G$1" pin="3"/>
</segment>
</net>
<net name="N$5" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="ALERT"/>
<wire x1="-15.24" y1="-2.54" x2="-12.7" y2="-2.54" width="0.1524" layer="91"/>
<wire x1="-15.24" y1="-2.54" x2="-15.24" y2="-12.7" width="0.1524" layer="91"/>
<wire x1="-15.24" y1="-12.7" x2="-7.62" y2="-12.7" width="0.1524" layer="91"/>
<wire x1="-7.62" y1="-12.7" x2="-7.62" y2="-30.48" width="0.1524" layer="91"/>
<pinref part="CON1" gate="G$1" pin="5"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
</eagle>
