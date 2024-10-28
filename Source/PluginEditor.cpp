/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
LModelAudioProcessorEditor::LModelAudioProcessorEditor(LModelAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.
	setResizable(true, true); // ÔÊÐí´°¿Úµ÷Õû´óÐ¡

	setOpaque(false);  // ÔÊÐíÔÚ±ß¿òÍâÃæ»æÖÆ

	//setResizeLimits(64 * 11, 64 * 5, 10000, 10000); // ÉèÖÃ×îÐ¡¿í¸ßÎª300x200£¬×î´ó¿í¸ßÎª800x600
	setSize(64 * 13, 64 * 4);

	//constrainer.setFixedAspectRatio(11.0 / 4.0);  // ÉèÖÃÎª16:9±ÈÀý
	//setConstrainer(&constrainer);  // °ó¶¨´°¿ÚµÄ¿í¸ßÏÞÖÆ

	K_FreqL.setText("FreqL");
	K_FreqL.ParamLink(audioProcessor.GetParams(), "freql");
	addAndMakeVisible(K_FreqL);

	K_FreqR.setText("FreqR");
	K_FreqR.ParamLink(audioProcessor.GetParams(), "freqr");
	addAndMakeVisible(K_FreqR);

	startTimerHz(30);

}

LModelAudioProcessorEditor::~LModelAudioProcessorEditor()
{
}

//==============================================================================
void LModelAudioProcessorEditor::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colour(0x00, 0x00, 0x00));

	g.fillAll();
	g.setFont(juce::Font("FIXEDSYS", 17.0, 1));
	g.setColour(juce::Colour(0xff00ff00));;

	int w = getBounds().getWidth(), h = getBounds().getHeight();

}

void LModelAudioProcessorEditor::resized()
{
	juce::Rectangle<int> bound = getBounds();
	int x = bound.getX(), y = bound.getY(), w = bound.getWidth(), h = bound.getHeight();
	auto convXY = juce::Rectangle<int>::leftTopRightBottom;

	K_FreqL.setBounds(32 + 64 * 0, 32 + 64 * 0, 64, 64);
	K_FreqR.setBounds(32 + 64 * 1, 32 + 64 * 0, 64, 64);
}

#define MaxFRDatLen (32768)
void LModelAudioProcessorEditor::timerCallback()
{
	repaint();
}
