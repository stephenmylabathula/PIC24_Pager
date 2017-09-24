package com.stephenmylabathula.iot.pic_pager;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.app.Activity;
import android.bluetooth.*;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;

import android.widget.*;

import java.io.*;
import java.util.*;

public class MainActivity extends AppCompatActivity {

    Button btnOn,btnOff,btnPair,btnSend;
    private BluetoothAdapter BA;
    private Set<BluetoothDevice>pairedDevices;
    EditText txtDevice, txtPayload;
    String temp;

    BluetoothDevice bd = null;
    BluetoothSocket bs = null;
    private OutputStream mmOutStream = null;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        btnOn = (Button) findViewById(R.id.buttonOn);
        btnOff=(Button)findViewById(R.id.buttonOff);
        btnPair=(Button)findViewById(R.id.buttonPair);
        btnSend=(Button)findViewById(R.id.buttonSend);

        BA = BluetoothAdapter.getDefaultAdapter();
        txtDevice = (EditText)findViewById(R.id.txtDeviceName);
        txtPayload = (EditText)findViewById(R.id.txtPayload);
    }

    public void on(View v){
        if (!BA.isEnabled()) {
            Intent turnOn = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(turnOn, 0);
            Toast.makeText(getApplicationContext(), "Turned on",Toast.LENGTH_LONG).show();
        } else {
            Toast.makeText(getApplicationContext(), "Already on", Toast.LENGTH_LONG).show();
        }
    }

    public void off(View v){
        BA.disable();
        Toast.makeText(getApplicationContext(), "Turned off" ,Toast.LENGTH_LONG).show();
        try {
            if (bs != null)
                bs.close();
        } catch(IOException e){

        }
    }


    public  void visible(View v){
        Intent getVisible = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
        startActivityForResult(getVisible, 0);
    }

    public void send(View v){
        try {
            try {
                mmOutStream = bs.getOutputStream();
                temp = txtPayload.getText().toString();
                mmOutStream.write(temp.getBytes());
                mmOutStream.write('\0');
                Toast.makeText(getApplicationContext(), "Sent",Toast.LENGTH_LONG).show();
            } catch (Exception ex){
                Toast.makeText(getApplicationContext(), "Failed",Toast.LENGTH_LONG).show();
            }
        } catch (Exception e){
            Toast.makeText(getApplicationContext(), "Failed",Toast.LENGTH_LONG).show();
        }


    }

    public void pair(View v){
        pairedDevices = BA.getBondedDevices();
        try{
        for(BluetoothDevice bt : pairedDevices) {
            if (bt.getName().equals(txtDevice.getText().toString())) {
                bd = BA.getRemoteDevice(bt.getAddress());
                Toast.makeText(getApplicationContext(), "Found",Toast.LENGTH_LONG).show();
            }
        }
            System.out.println(bd.getUuids()[0].getUuid());
            bs = bd.createInsecureRfcommSocketToServiceRecord(bd.getUuids()[0].getUuid());
            BA.cancelDiscovery();
            bs.connect();
        } catch (Exception e){
            Toast.makeText(getApplicationContext(), "Error in Pair",Toast.LENGTH_LONG).show();
        }

    }


}
