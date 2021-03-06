/*******************************************************************************
 *
 *                                 Silverjuke
 *     Copyright (C) 2016 Björn Petersen Software Design and Development
 *                   Contact: r10s@b44t.com, http://b44t.com
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see http://www.gnu.org/licenses/ .
 *
 *******************************************************************************
 *
 * File:    wavework.cpp
 * Authors: Björn Petersen
 * Purpose: Working with waves
 *
 ******************************************************************************/


#include <sjbase/base.h>
#include <sjbase/backend.h>


double SjDecibel2Gain(double db)
{
	// for some common parameters, we're using predefined
	// return values to avoid rounding errors
	if( db == 14.00 )
	{
		return 5.0;
	}
	else if( db == 12.00 )
	{
		return 4.0;
	}
	else
	{
		return pow(10.0, db / 20.0);
	}
}


double SjGain2Decibel(double gain)
{
	// for some common parameters, we're using predefined
	// return values to avoid rounding errors
	if( gain == 5.0 )
	{
		return 14.0;
	}
	else if( gain > 0.0 )
	{
		return (20.0 * log10(gain));
	}
	else
	{
		return 0.0;
	}
}


long SjGain2Long(double gain)
{
	if( gain > 0 )
	{
		return (long)(gain*1000.0F);
	}
	else
	{
		return 0;
	}
}


double SjLong2Gain(long lng)
{
	return (double) (((double)lng) / 1000.0F);
}


void SjApplyVolume(float* buffer, long bytes, float gain)
{
	const float* bufferEnd = buffer + (bytes/sizeof(float));
	while( buffer < bufferEnd ) {
		*buffer++ *= gain;
	}
}


void SjMixdownChannels(float* buffer, long bytes, int channels, int destCh)
{
	// in a buffer defined by buffer-bytes-channels, mix all channels to destCh and mute the other ones
	if( channels <= 1 || channels > 256 || destCh < 0 || destCh >= channels ) return; // error

	float subsamsSum;
	long sampleStart, subsam, subsams = bytes / sizeof(float);
	for( sampleStart = 0; sampleStart < subsams; sampleStart += channels )
	{
		subsamsSum = 0;
		for( subsam = 0; subsam < channels; subsam++ )
		{
			subsamsSum += buffer[sampleStart+subsam];
			buffer[sampleStart+subsam] = 0;
		}
		buffer[sampleStart+destCh] = subsamsSum / channels;
	}
}


void SjFloatToPcm16(const float* fBuf, signed short* sBuf, long numBytes)
{
	// copy forward to allow using the same buffers
	long numSamples = numBytes / sizeof(float);
	const float* fBufEnd = &fBuf[numSamples];
	float sample;
	while( fBuf < fBufEnd )
	{
		sample = *fBuf * 32767.0F;
		if( sample < -32768.0F ) sample = -32768.0F;
		if( sample >  32767.0F ) sample =  32767.0F;
		*sBuf = (signed short)sample;

		fBuf++;
		sBuf++;
	}
}


void SjPcm16ToFloat(const signed short* sBufStart, float* fBufStart, long numBytes)
{
	// copy backward to allow using the same buffers
	// note, that fBuf needs to be two times larger than sBuf
	long numSamples = numBytes / sizeof(signed short);
	const signed short* s = &sBufStart[numSamples-1];
	float* f = &fBufStart[numSamples-1];
	while( s >= sBufStart )
	{
		*f = (float)(*s) / 32767.0F;
		f--;
		s--;
	}
}

