package fr.marcworld.tourox.android;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothProfile;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.ActionBarActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.TimePicker;
import android.widget.Toast;

import fr.marcworld.tourox.android.R;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;
import java.util.UUID;


public class DeviceActivity extends ActionBarActivity {

    private static final int AVERAGE_RSSI_AT_1_METER_FROM_DEVICE = -55;
    private Handler handler;
    private Button connectButton;
    private volatile boolean connected = false;
    private BluetoothDevice device;
    private GGTABluetoothGattCallback ggtaBluetoothGattCallback;
    private BluetoothGatt bluetoothGatt;
    private BluetoothGattCharacteristic writeBluetoothGattCharacteristic;
    private TextView helpRequestTextView;
    private Button setCurrentProximityLevelButton = null;
    private LinearLayout requestAndCommandsLinearLayout;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_device);

        handler = new Handler();
        requestAndCommandsLinearLayout = (LinearLayout) findViewById(R.id.requestAndCommandsLinearLayout);
        requestAndCommandsLinearLayout.setEnabled(false);

        //
        // Show the selected device information
        //
        Intent intent = getIntent();
        device = intent.getParcelableExtra(MainActivity.EXTRA_BLUETOOTH_DEVICE);
        byte[] scanRecord = intent.getByteArrayExtra(MainActivity.EXTRA_BLUETOOTH_SCAN_RECORD);

        TextView bleDeviceNameTextView = (TextView) findViewById(R.id.bleDeviceName);
        bleDeviceNameTextView.setText(ScannerServiceParser.decodeDeviceName(scanRecord));

        TextView bleDeviceDetailsTextView = (TextView) findViewById(R.id.bleDeviceDetails);
        bleDeviceDetailsTextView.setText(device.getAddress());

        connectButton = (Button) findViewById(R.id.connectButton);
        connectButton.setEnabled(false);
        connectButton.setText(R.string.connecting_button);
        connectButton.setOnClickListener(new ConnectButtonListener());

        //
        // Commands panel
        //

        // Set time command
        final EditText fixedTimeEditText = (EditText) findViewById(R.id.fixedTimeEditText);
        Calendar calendar = Calendar.getInstance();
        fixedTimeEditText.setText(calendar.get(Calendar.HOUR_OF_DAY) + ":" + calendar.get(Calendar.MINUTE) + ":" + calendar.get(Calendar.SECOND));

        Button setCurrentTimeButton = (Button) findViewById(R.id.setCurrentTimeButton);
        setCurrentTimeButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendSetTimeCommand(Calendar.getInstance());
            }
        });

        Button setFixedTimeButton = (Button) findViewById(R.id.setFixedTimeButton);
        setFixedTimeButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String fixedTime = String.valueOf(fixedTimeEditText.getText());
                String hourAsString = fixedTime.substring(0, fixedTime.indexOf(':'));
                String minuteAsString = fixedTime.substring(fixedTime.indexOf(':') + 1, fixedTime.lastIndexOf(':'));
                String secondAsString = fixedTime.substring(fixedTime.lastIndexOf(':') + 1);

                Calendar calendar = Calendar.getInstance();
                calendar.set(Calendar.HOUR_OF_DAY, Integer.valueOf(hourAsString));
                calendar.set(Calendar.MINUTE, Integer.valueOf(minuteAsString));
                calendar.set(Calendar.SECOND, Integer.valueOf(secondAsString));
                sendSetTimeCommand(calendar);
            }
        });

        // Set proximity Level command
        final Spinner fixedProximityLevelSpinner = (Spinner) findViewById(R.id.fixedProximityLevelSpinner);
        ArrayAdapter<CharSequence> fixedProximityLevelAdapter = ArrayAdapter.createFromResource(DeviceActivity.this, R.array.proximity_level_array, android.R.layout.simple_spinner_item);
        fixedProximityLevelAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        fixedProximityLevelSpinner.setAdapter(fixedProximityLevelAdapter);

        Button setFixedProximityLevelButton = (Button) findViewById(R.id.setFixedProximityLevelButton);
        setFixedProximityLevelButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendSetProximityLevelCommand((byte) (fixedProximityLevelSpinner.getSelectedItemPosition() + 1));
            }
        });

        setCurrentProximityLevelButton = (Button) findViewById(R.id.setCurrentProximityLevelButton);
        setCurrentProximityLevelButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                bluetoothGatt.readRemoteRssi();
                setCurrentProximityLevelButton.setEnabled(false);
                setCurrentProximityLevelButton.setText(R.string.setting_current_proximity_level_button);
            }
        });

        // Set Tour Information command
        Button setTourInfoButton = (Button) findViewById(R.id.setTourInfoButton);
        final EditText simCardPinNumberEditText = (EditText) findViewById(R.id.simCardPinNumberEditText);
        final EditText guidePhoneNumberEditText = (EditText) findViewById(R.id.guidePhoneNumberEditText);
        final EditText apnEditText = (EditText) findViewById(R.id.apnEditText);
        final EditText apnUsernameEditText = (EditText) findViewById(R.id.apnUsernameEditText);
        final EditText apnPasswordEditText = (EditText) findViewById(R.id.apnPasswordEditText);
        setTourInfoButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendSetTourInfoCommand(
                        String.valueOf(simCardPinNumberEditText.getText()),
                        String.valueOf(guidePhoneNumberEditText.getText()),
                        String.valueOf(apnEditText.getText()),
                        String.valueOf(apnUsernameEditText.getText()),
                        String.valueOf(apnPasswordEditText.getText()));
            }
        });

        // Clear Tour Information command
        Button clearTourInfoButton = (Button) findViewById(R.id.clearTourInfoButton);
        clearTourInfoButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendClearTourInfoCommand();
            }
        });

        // Cancel Help Request
        Button cancelHelpRequestButton = (Button) findViewById(R.id.cancelHelpRequestButton);
        cancelHelpRequestButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendCancelHelpRequestCommand();
            }
        });

        // Schedule a Gathering
        final TimePicker gatheringTimePicker = (TimePicker) findViewById(R.id.gatheringTimePicker);
        gatheringTimePicker.setIs24HourView(Boolean.TRUE);
        calendar = Calendar.getInstance();
        gatheringTimePicker.setCurrentHour(calendar.get(Calendar.HOUR_OF_DAY));
        gatheringTimePicker.setCurrentMinute(calendar.get(Calendar.MINUTE));

        final EditText gatheringDescriptionEditText = (EditText) findViewById(R.id.gatheringDescriptionEditText);

        Button scheduleGatheringButton = (Button) findViewById(R.id.scheduleGatheringButton);
        scheduleGatheringButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendScheduleGatheringCommand(String.valueOf(gatheringDescriptionEditText.getText()), gatheringTimePicker.getCurrentHour(), gatheringTimePicker.getCurrentMinute());
            }
        });

        // Unschedule a Gathering
        Button unscheduleGatheringButton = (Button) findViewById(R.id.unscheduleGatheringButton);
        unscheduleGatheringButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendUnscheduleGatheringCommand();
            }
        });

        // Connect to the device GATT server
        ggtaBluetoothGattCallback = new GGTABluetoothGattCallback();
        bluetoothGatt = device.connectGatt(this, false, ggtaBluetoothGattCallback);
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_device, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private class GGTABluetoothGattCallback extends BluetoothGattCallback {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        connectButton.setText(R.string.disconnect_button);
                        connectButton.setEnabled(true);
                        requestAndCommandsLinearLayout.setEnabled(true);
                        connected = true;
                    }
                });

                bluetoothGatt.discoverServices();
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        connectButton.setText(R.string.connect_button);
                        connectButton.setEnabled(true);
                        requestAndCommandsLinearLayout.setEnabled(false);
                        connected = false;
                    }
                });
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            // Find the custom service
            List<BluetoothGattService> services = gatt.getServices();
            BluetoothGattService customService = null;
            for (BluetoothGattService service : services) {
                if (service.getUuid().toString().startsWith("0000a000")) {
                    customService = service;
                    break;
                }
            }

            // Find the custom characteristics
            if (customService != null) {
                writeBluetoothGattCharacteristic = null;
                List<BluetoothGattCharacteristic> characteristics = customService.getCharacteristics();
                for (BluetoothGattCharacteristic characteristic : characteristics) {
                    if (characteristic.getUuid().toString().startsWith("0000a002")) {
                        writeBluetoothGattCharacteristic = characteristic;
                    }
                    if (characteristic.getUuid().toString().startsWith("0000a001")) {
                        gatt.setCharacteristicNotification(characteristic, true);

                        BluetoothGattDescriptor descriptor = characteristic.getDescriptor(UUID.fromString("00002902-0000-1000-8000-00805f9b34fb"));
                        descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
                        gatt.writeDescriptor(descriptor);
                    }
                }
            }
        }

        @Override
        public void onReadRemoteRssi(BluetoothGatt gatt, int rssi, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                if (setCurrentProximityLevelButton != null && !setCurrentProximityLevelButton.isEnabled()) {
                    setCurrentProximityLevelButton.setEnabled(true);
                    setCurrentProximityLevelButton.setText(R.string.set_current_proximity_level_button);

                    double distanceInMeters = calculateAccuracy(AVERAGE_RSSI_AT_1_METER_FROM_DEVICE, rssi);
                    if (distanceInMeters <= 1) {
                        sendSetProximityLevelCommand((byte) 5);
                    } else if (distanceInMeters <= 2) {
                        sendSetProximityLevelCommand((byte) 4);
                    } else if (distanceInMeters <= 3) {
                        sendSetProximityLevelCommand((byte) 3);
                    } else if (distanceInMeters <= 4) {
                        sendSetProximityLevelCommand((byte) 2);
                    } else {
                        sendSetProximityLevelCommand((byte) 1);
                    }
                }
            }
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            if (characteristic.getUuid().toString().startsWith("0000a001")) {
                bluetoothGatt.readCharacteristic(characteristic);
                byte[] message = characteristic.getValue();

                if (message != null && message.length > 1) {
                    /*switch (message[0]) {
                        case 'H':
                            onHelpRequested();
                            break;
                        case 'C':
                            onHelpRequestCancelled();
                            break;
                    }*/
                }
            }
        }
    }

    private class ConnectButtonListener implements View.OnClickListener {
        @Override
        public void onClick(View v) {
            if (connected) {
                bluetoothGatt.disconnect();
            } else {
                connectButton.setEnabled(false);
                connectButton.setText(R.string.connecting_button);
                bluetoothGatt = device.connectGatt(DeviceActivity.this, false, ggtaBluetoothGattCallback);
            }
        }
    }

    private void sendSetTimeCommand(Calendar calendar) {
        byte[] setTimeCommand = {'T', (byte) calendar.get(Calendar.HOUR_OF_DAY), (byte) calendar.get(Calendar.MINUTE), (byte) calendar.get(Calendar.SECOND)};
        writeBluetoothGattCharacteristic.setValue(setTimeCommand);
        bluetoothGatt.writeCharacteristic(writeBluetoothGattCharacteristic);
    }

    private void sendSetProximityLevelCommand(byte level) {
        byte[] setProximityLevelCommand = {'P', level};
        writeBluetoothGattCharacteristic.setValue(setProximityLevelCommand);
        bluetoothGatt.writeCharacteristic(writeBluetoothGattCharacteristic);
    }

    private void sendSetTourInfoCommand(String simCardPinCode, String guidePhoneNumber, String apn, String apnUsername, String apnPassword) {
        if (simCardPinCode.length() != 4) {
            Toast.makeText(this, "Error: the SIM pin code must have 4 characters.", Toast.LENGTH_LONG).show();
            return;
        }
        if (guidePhoneNumber.length() < 5) {
            Toast.makeText(this, "Error: the phone number must have more than 5 characters.", Toast.LENGTH_LONG).show();
            return;
        }
        if (guidePhoneNumber.length() > 31) {
            Toast.makeText(this, "Error: the phone number must have less than 31 characters.", Toast.LENGTH_LONG).show();
            return;
        }
        if (apn.length() < 3) {
            Toast.makeText(this, "Error: the APN must have more than 3 characters.", Toast.LENGTH_LONG).show();
            return;
        }
        byte[] setTourInfoCommand = new byte[1 + simCardPinCode.length() + 1 + guidePhoneNumber.length() + 1 + apn.length() + 1 + apnUsername.length() + 1 + apnPassword.length()];
        setTourInfoCommand[0] = 'I';
        setTourInfoCommand[1] = (byte) simCardPinCode.charAt(0);
        setTourInfoCommand[2] = (byte) simCardPinCode.charAt(1);
        setTourInfoCommand[3] = (byte) simCardPinCode.charAt(2);
        setTourInfoCommand[4] = (byte) simCardPinCode.charAt(3);
        setTourInfoCommand[5] = (byte) guidePhoneNumber.length();
        for (int i = 0; i < guidePhoneNumber.length(); i++) {
            setTourInfoCommand[6 + i] = (byte) guidePhoneNumber.charAt(i);
        }
        setTourInfoCommand[6 + guidePhoneNumber.length()] = (byte) apn.length();
        for (int i = 0; i < apn.length(); i++) {
            setTourInfoCommand[7 + guidePhoneNumber.length() + i] = (byte) apn.charAt(i);
        }
        setTourInfoCommand[7 + guidePhoneNumber.length() + apn.length()] = (byte) apnUsername.length();
        for (int i = 0; i < apnUsername.length(); i++) {
            setTourInfoCommand[8 + guidePhoneNumber.length() + apn.length() + i] = (byte) apnUsername.charAt(i);
        }
        setTourInfoCommand[8 + guidePhoneNumber.length() + apn.length() + apnUsername.length()] = (byte) apnPassword.length();
        for (int i = 0; i < apnPassword.length(); i++) {
            setTourInfoCommand[9 + guidePhoneNumber.length() + apn.length() + apnUsername.length() + i] = (byte) apnPassword.charAt(i);
        }
        sendCommandInChunks(setTourInfoCommand);
    }

    private void sendClearTourInfoCommand() {
        byte[] clearTourInfoCommand = {'C'};
        writeBluetoothGattCharacteristic.setValue(clearTourInfoCommand);
        bluetoothGatt.writeCharacteristic(writeBluetoothGattCharacteristic);
    }

    private void sendCancelHelpRequestCommand() {
        byte[] cancelHelpRequestCommand = {'A'};
        writeBluetoothGattCharacteristic.setValue(cancelHelpRequestCommand);
        bluetoothGatt.writeCharacteristic(writeBluetoothGattCharacteristic);
    }

    private void sendScheduleGatheringCommand(String description, int hour, int minute) {
        try {
            byte[] descriptionInAscii = description.getBytes("US-ASCII");
            int descriptionLength = descriptionInAscii.length > 80 ? 80 : descriptionInAscii.length;
            byte[] scheduleGatheringCommand = new byte[descriptionLength + 5];
            scheduleGatheringCommand[0] = 'G';
            scheduleGatheringCommand[1] = (byte) hour;
            scheduleGatheringCommand[2] = (byte) minute;
            scheduleGatheringCommand[3] = (byte) descriptionLength;
            System.arraycopy(descriptionInAscii, 0, scheduleGatheringCommand, 4, descriptionLength);
            scheduleGatheringCommand[scheduleGatheringCommand.length - 1] = 0;

            sendCommandInChunks(scheduleGatheringCommand);
        } catch (final UnsupportedEncodingException e) {
            handler.post(new Runnable() {
                @Override
                public void run() {
                    Toast.makeText(DeviceActivity.this, "Unable to convert the gathering description in ASCII code: " + e.getMessage(), Toast.LENGTH_LONG).show();
                }
            });
        }
    }

    private void sendUnscheduleGatheringCommand() {
        byte[] unscheduleGatheringCommand = {'U'};
        writeBluetoothGattCharacteristic.setValue(unscheduleGatheringCommand);
        bluetoothGatt.writeCharacteristic(writeBluetoothGattCharacteristic);
    }

    private void sendCommandInChunks(byte[] command) {
        if (command == null || command.length <= 20) {
            throw new IllegalArgumentException("Do not send the command in chunk if it is smaller than 20 bytes.");
        }
        List<byte[]> chunks = new ArrayList<>((int) Math.ceil((double)command.length / 20.0));
        byte[] currentChunk = null;
        int currentChunkIndex = 0;
        for (int i = 0; i < command.length; i++) {
            if (currentChunk == null || currentChunkIndex >= 20) {
                currentChunk = new byte[20];
                chunks.add(currentChunk);

                if(chunks.size() == 1) { // First chunk
                    currentChunk[0] = 'B';
                } else {
                    currentChunk[0] = 'O';
                }
                currentChunkIndex = 1;
            }

            currentChunk[currentChunkIndex] = command[i];
            currentChunkIndex++;
        }
        currentChunk[0] = 'E';

        for (byte[] chunk : chunks) {
            writeBluetoothGattCharacteristic.setValue(chunk);
            bluetoothGatt.writeCharacteristic(writeBluetoothGattCharacteristic);
            try { Thread.sleep(200); } catch (InterruptedException e) { }
        }
    }

    /**
     * See http://stackoverflow.com/questions/20416218/understanding-ibeacon-distancing/20434019#20434019
     * See http://stackoverflow.com/a/22787597
     */
    private static double calculateAccuracy(int txPower, double rssi) {
        if (rssi == 0) {
            return -1.0; // if we cannot determine accuracy, return -1.
        }

        double ratio = rssi*1.0/txPower;
        if (ratio < 1.0) {
            return Math.pow(ratio,10);
        }
        else {
            double accuracy =  (0.89976)*Math.pow(ratio,7.7095) + 0.111;
            return accuracy;
        }
    }
}
