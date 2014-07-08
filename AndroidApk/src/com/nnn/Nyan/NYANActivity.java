package com.nnn.Nyan;

import android.os.Bundle;
import com.nnn.lib.nnnView;
import com.nnn.lib.nnnActivity;

public class NYANActivity extends nnnActivity
{
	static
	{
		System.loadLibrary("Nyan");
	}

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
	}

	@Override
	protected nnnView CreateView()
	{
		return new nnnView(this, 16, 0);
	}
}
