package fr.marcworld.tourox.android;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBarActivity;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;


public class MainActivity extends ActionBarActivity {

    public final static String EXTRA_BLUETOOTH_DEVICE = "fr.marcworld.tourox.android.BLUETOOTH_DEVICE";
    public final static String EXTRA_BLUETOOTH_SCAN_RECORD = "fr.marcworld.tourox.android.BLUETOOTH_SCAN_RECORD";
    private static final int REQUEST_ENABLE_BT = 1;
    private static final long DEVICE_SCANNING_PERIOD_MILLIS = 10000; // Stops scanning after 10 seconds.

    private boolean scanningDevices = false;
    private BluetoothAdapter bluetoothAdapter;
    private BluetoothDeviceScanCallback bluetoothDeviceScanCallback;
    private Handler handler;
    private Button scanButton;
    private ScannedBluetoothDeviceArrayAdapter scannedBluetoothDeviceArrayAdapter;
    private final List<ScannedBluetoothDevice> scannedBluetoothDevices = Collections.synchronizedList(new ArrayList<ScannedBluetoothDevice>());

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        handler = new Handler();
        bluetoothDeviceScanCallback = new BluetoothDeviceScanCallback();

        // Prepare widgets
        scanButton = (Button) findViewById(R.id.scanButton);
        scanButton.setOnClickListener(new ScanButtonClickListener());
        scannedBluetoothDeviceArrayAdapter = new ScannedBluetoothDeviceArrayAdapter(this, R.id.availableDeviceListView);
        ListView availableDeviceListView = (ListView) findViewById(R.id.availableDeviceListView);
        availableDeviceListView.setAdapter(scannedBluetoothDeviceArrayAdapter);

        // Initializes Bluetooth adapter.
        final BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        bluetoothAdapter = bluetoothManager.getAdapter();

        // Ensures Bluetooth is available on the device and it is enabled. If not,
        // displays a dialog requesting user permission to enable Bluetooth.
        if (bluetoothAdapter == null || !bluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        } else {
            onBluetoothSetupDone();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {
            case REQUEST_ENABLE_BT:
                if (resultCode == RESULT_OK) {
                    onBluetoothSetupDone();
                } else {
                    Toast.makeText(this, R.string.error_ble_unavailable, Toast.LENGTH_LONG).show();
                }
                break;

            default:
                super.onActivityResult(requestCode, resultCode, data);
                break;
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
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

    /**
     * Method called when the bluetooth is on.
     */
    private void onBluetoothSetupDone() {
        // Enable the scan button
        scanButton.setEnabled(true);
    }

    /**
     * Activate or not bluetooth device scanning.
     */
    private void setBleDeviceScanningActive(boolean active) {
        if (active) {
            scanningDevices = true;
            scanButton.setText(R.string.scanning_button);
            scanButton.setEnabled(false);
            bluetoothAdapter.startLeScan(bluetoothDeviceScanCallback);
        } else {
            scanningDevices = false;
            scanButton.setText(R.string.scan_button);
            scanButton.setEnabled(true);
            bluetoothAdapter.stopLeScan(bluetoothDeviceScanCallback);
        }
    }

    private class ScanButtonClickListener implements View.OnClickListener {
        @Override
        public void onClick(View v) {
            // Activate or deactivate BLE device scanning
            if (!scanningDevices) {
                setBleDeviceScanningActive(true);

                // Stops scanning after a pre-defined scan period.
                handler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        setBleDeviceScanningActive(false);
                    }
                }, DEVICE_SCANNING_PERIOD_MILLIS);

            } else {
                setBleDeviceScanningActive(false);
            }
        }
    }

    private class BluetoothDeviceScanCallback implements BluetoothAdapter.LeScanCallback {
        @Override
        public void onLeScan(final BluetoothDevice device, final int rssi, final byte[] scanRecord) {
            handler.post(new Runnable() {
                @Override
                public void run() {
                    ScannedBluetoothDevice scannedBluetoothDevice = new ScannedBluetoothDevice(device, rssi, scanRecord);
                    scannedBluetoothDevices.remove(scannedBluetoothDevice);
                    scannedBluetoothDevices.add(scannedBluetoothDevice);
                    Collections.sort(scannedBluetoothDevices);
                    scannedBluetoothDeviceArrayAdapter.notifyDataSetChanged();
                }
            });
        }
    }

    private class ScannedBluetoothDeviceArrayAdapter extends ArrayAdapter<ScannedBluetoothDevice> {

        ScannedBluetoothDeviceArrayAdapter(Context context, int resource) {
            super(context, resource, scannedBluetoothDevices);
        }

        @NonNull
        @Override
        public View getView(int position, View convertView, @NonNull ViewGroup parent) {
            LayoutInflater inflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            View rowView = inflater.inflate(R.layout.ble_device_list_item, parent, false);

            ScannedBluetoothDevice scannedBluetoothDevice = scannedBluetoothDevices.get(position);

            TextView bleDeviceNameTextView = (TextView) rowView.findViewById(R.id.bleDeviceName);
            bleDeviceNameTextView.setText(ScannerServiceParser.decodeDeviceName(scannedBluetoothDevice.scanRecord));

            TextView bleDeviceDetailsTextView = (TextView) rowView.findViewById(R.id.bleDeviceDetails);
            bleDeviceDetailsTextView.setText(scannedBluetoothDevice.device.getAddress() + "   " + scannedBluetoothDevice.rssi + "dBm");

            Button connectButton = (Button) rowView.findViewById(R.id.connectButton);
            connectButton.setOnClickListener(new ConnectButtonListener(scannedBluetoothDevice));

            return rowView;
        }
    }

    private static class ScannedBluetoothDevice implements Comparable<ScannedBluetoothDevice> {
        private final BluetoothDevice device;
        private final int rssi;
        private final byte[] scanRecord;

        private ScannedBluetoothDevice(BluetoothDevice device, int rssi, byte[] scanRecord) {
            this.device = device;
            this.rssi = rssi;
            this.scanRecord = scanRecord;
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;

            ScannedBluetoothDevice that = (ScannedBluetoothDevice) o;

            return device.getAddress().equals(that.device.getAddress());
        }

        @Override
        public int hashCode() {
            return device.getAddress().hashCode();
        }

        @Override
        public int compareTo(@NonNull ScannedBluetoothDevice another) {
            return another.rssi - this.rssi;
        }
    }

    private class ConnectButtonListener implements View.OnClickListener {

        private final ScannedBluetoothDevice scannedBluetoothDevice;

        private ConnectButtonListener(ScannedBluetoothDevice scannedBluetoothDevice) {
            this.scannedBluetoothDevice = scannedBluetoothDevice;
        }

        @Override
        public void onClick(View v) {
            Intent intent = new Intent(MainActivity.this, DeviceActivity.class);
            intent.putExtra(EXTRA_BLUETOOTH_DEVICE, scannedBluetoothDevice.device);
            intent.putExtra(EXTRA_BLUETOOTH_SCAN_RECORD, scannedBluetoothDevice.scanRecord);
            startActivity(intent);
        }
    }
}
